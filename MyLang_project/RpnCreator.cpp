#include "RpnCreator.h"

class RpnCreator::Tid {
private:
    Tid* parent_;
    std::set<int> vars_;
    bool is_loop_, is_function_;
public:
    Tid(Tid* parent = NULL, bool is_loop = false, bool is_function = false) : parent_(parent), is_loop_(is_loop), is_function_(is_function) {}
    void setVar(int var) {
        vars_.insert(var);
    }
    Tid* parent() { return parent_; }
    bool isEmpty() { return vars_.empty(); }
    int getVar() {
        int var = *vars_.begin();
        vars_.erase(vars_.begin());
        return var;
    }
    std::set<int> getVars() { return vars_; }
    bool isLoop() { return is_loop_; }
    bool isFunction() { return is_function_; }
};



void RpnCreator::pushRpn(RPN::Token* token) {
    rpn_.push_back(token);
    cur_rpn_++;
}

void RpnCreator::setRpn(int it, RPN::Token* token) {
    delete rpn_[it];
    rpn_[it] = token;
}

void RpnCreator::nextTid(bool is_loop, bool is_function) {
    cur_tid_ = new Tid(cur_tid_, is_loop, is_function);
}

void RpnCreator::prevTid() {

    while (!cur_tid_->isEmpty()) {
        int var = cur_tid_->getVar();
        if (var != 0)
            pushRpn(new RPN::DeleteVar(var));
    }

    cur_tid_ = cur_tid_->parent();
}




LecsicalToken RpnCreator::token() { return tokens_[cur_token_]; }
std::string RpnCreator::tokenName() { return tokens_[cur_token_].name(); }
LecsicalEnum RpnCreator::tokenType() { return tokens_[cur_token_].type(); }
void RpnCreator::tokenNext() { cur_token_++; }
void RpnCreator::tokenPrev() { cur_token_--; }
std::string RpnCreator::tokenFile() { return tokens_[cur_token_].file_name(); }
int RpnCreator::tokenLine() { return tokens_[cur_token_].lineNumber(); }
int RpnCreator::tokenChar() { return tokens_[cur_token_].charNumber(); }

bool RpnCreator::isType(LecsicalToken token) {
    return token.name() == "bool" || token.name() == "char" ||
        token.name() == "int" || token.name() == "float" || token.name() == "string" || token.name() == "range" || token.name() == "array";
}

RpnCreator::RpnCreator(std::vector<LecsicalToken> tokens, std::queue<FuncId> function_calls, std::queue<MethodId> method_calls) :
    tokens_(tokens), function_calls_(function_calls), method_calls_(method_calls), cur_token_(0), cur_rpn_(0), cur_tid_(new Tid()) {
    mc_copy = method_calls;
    fc_copy = function_calls;
}

std::vector<RPN::Token*> RpnCreator::getTokens() { return rpn_; }

int RpnCreator::getMain() { return main_start; }

int RpnCreator::varNumber() { return var_nums_.size(); }

void RpnCreator::work() {
    if (tokens_.size() == 0) return;
    tokens_.push_back(LecsicalToken("", LecsicalEnum::NONE, 0, 0, ""));
    var_nums_["it"] = 0;

    prev_global_var = cur_rpn_;
    pushRpn(new RPN::Goto());

    while (tokenType() != LecsicalEnum::NONE) {
        int start = cur_token_;

        if (tokenName() == "void") {
            tokenNext(); // from void to smth
            if (tokenType() == LecsicalEnum::NAME) {
                cur_token_ = start;
                FUNCTION_DECLARATION();
            }
            else {
                cur_token_ = start;
                METHOD_DECLARATION();
            }
            continue;
        }

        TYPE(); // from type to smth

        if (tokenType() == LecsicalEnum::NAME) {
            tokenNext(); // from name to smth
            if (tokenName() == "(") {
                cur_token_ = start;
                FUNCTION_DECLARATION();
            }
            else {
                cur_token_ = start;
                VARIABLE_DECLARATION();
            }
        }
        else {
            cur_token_ = start;
            METHOD_DECLARATION();
        }
    }
    setCalls();
    setRpn(prev_global_var, new RPN::Goto(main_start));
}

Type* RpnCreator::TYPE() {
    if (isType(token()) && tokenName() != "array") {
        Type* ret = new Type(tokenName());
        tokenNext(); // from type to smth
        return ret;
    }
    tokenNext(); // from array to <
    tokenNext(); // from < to smth
    Type* base = TYPE(); // from type to >
    tokenNext(); // from > to smth
    return new Type("array", base);
}

void RpnCreator::FUNCTION_DECLARATION() {
    int start = cur_rpn_;
    nextTid(false, true);

    int func_start = cur_rpn_;

    Type* func_type = NULL;

    if (tokenName() != "void") {
        func_type = TYPE(); // from type to name
    }
    else {
        func_type = new Type();
        tokenNext(); // from void to name
    }

    std::string func_name = tokenName();
    tokenNext(); // from name to (

    std::vector<Type*> func_args;
    bool defaults = false;

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to arg
        if (tokenName() == ")") break;
        std::pair<Type*, bool> arg = FUNCTION_ARGUMENT();
        defaults = defaults || arg.second;
        if (defaults) {
            FuncId func(func_name, func_args);
            funcs_[func] = func_start;
        }
        func_args.push_back(arg.first);
    }
    FuncId func(func_name, func_args);
    funcs_[func] = func_start;

    tokenNext(); // from ) to {

    pushRpn(new RPN::DelArgCnt());

    nextTid();
    COMPOUND_OPERATOR(); // from compound to smth
    prevTid();
    prevTid();

    Type::TypeEnum base = func_type->baseType();

    Data::Data* ret = NULL;

    switch (base)
    {
    case Type::TypeEnum::BOOL:
        ret = new Data::Bool();
        break;
    case Type::TypeEnum::CHAR:
        ret = new Data::Char();
        break;
    case Type::TypeEnum::INT:
        ret = new Data::Int();
        break;
    case Type::TypeEnum::FLOAT:
        ret = new Data::Float();
        break;
    case Type::TypeEnum::STRING:
        ret = new Data::String();
        break;
    case Type::TypeEnum::RANGE_2:
        ret = new Data::Range();
        break;
    case Type::TypeEnum::RANGE_3:
        ret = new Data::Range();
        break;
    case Type::TypeEnum::ARRAY:
        ret = new Data::Array();
        break;
    case Type::TypeEnum::VOID:
        ret = new Data::Bool();
        break;
    default:
        break;
    }

    pushRpn(new RPN::Const(ret));
    
    pushRpn(new RPN::ReturnGoto());

    if (func_name == "main") {
        pushRpn(new RPN::End());
        main_start = start;
    }
}

void RpnCreator::METHOD_DECLARATION() {
    nextTid(false, true);
    int method_start = cur_rpn_;


    Type* method_type;
    if (tokenName() != "void") {
        method_type = TYPE(); // from type to type
    }
    else {
        tokenNext(); // from void to smth
        method_type = new Type("void");
    }

    if (var_nums_.find("it") == var_nums_.end()) {
        int size = var_nums_.size();
        var_nums_["it"] = size;
    }

    cur_tid_->setVar(var_nums_["it"]);

    Type* method_name_type = TYPE(); // from type to .
    tokenNext(); // from . to name
    std::string method_name = tokenName();
    tokenNext(); // from name to (

    std::vector<Type*> method_args;
    bool defaults = false;

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to arg
        if (tokenName() == ")") break;
        std::pair<Type*, bool> arg = FUNCTION_ARGUMENT(); // from arg to , or )
        defaults = defaults || arg.second;
        if (defaults) {
            MethodId method(method_name_type, method_name, method_args);
            methods_[method] = method_start;
        }
    }
    MethodId method(method_name_type, method_name, method_args);
    methods_[method] = method_start;

    tokenNext(); // from ( to {

    pushRpn(new RPN::DelArgCnt());
    pushRpn(new RPN::SetIt());

    nextTid();
    COMPOUND_OPERATOR(); // from compound to smth
    prevTid();
    prevTid();

    Type::TypeEnum base = method_type->baseType();

    Data::Data* ret = NULL;

    switch (base)
    {
    case Type::TypeEnum::BOOL:
        ret = new Data::Bool();
        break;
    case Type::TypeEnum::CHAR:
        ret = new Data::Char();
        break;
    case Type::TypeEnum::INT:
        ret = new Data::Int();
        break;
    case Type::TypeEnum::FLOAT:
        ret = new Data::Float();
        break;
    case Type::TypeEnum::STRING:
        ret = new Data::String();
        break;
    case Type::TypeEnum::RANGE_2:
        ret = new Data::Range();
        break;
    case Type::TypeEnum::RANGE_3:
        ret = new Data::Range();
        break;
    case Type::TypeEnum::ARRAY:
        ret = new Data::Array();
        break;
    case Type::TypeEnum::VOID:
        ret = new Data::Bool();
        break;
    default:
        break;
    }

    pushRpn(new RPN::Const(ret));
    pushRpn(new RPN::ReturnGoto());

}

std::pair<Type*, bool> RpnCreator::FUNCTION_ARGUMENT() {
    Type* type = TYPE(); // from type to name
    bool is_default = false;
    std::string arg_name = tokenName();
    tokenNext(); // from name to smth

    if (var_nums_.find(arg_name) == var_nums_.end()) {
        int var_size = var_nums_.size();
        var_nums_[arg_name] = var_size;
    }

    cur_tid_->setVar(var_nums_[arg_name]);

    pushRpn(new RPN::DeclareVar(var_nums_[arg_name], type));

    if (tokenName() == "=") {
        tokenPrev(); // from = to name
        EXPRESSION(); // from expression to smth
        pushRpn(new RPN::Kill());
        is_default = true;
    }
    pushRpn(new RPN::GetArg(var_nums_[arg_name]));

    return { type, is_default };
}


void RpnCreator::VARIABLE_DECLARATION() {
    if (cur_tid_->parent() == NULL) {
        setRpn(prev_global_var, new RPN::Goto(cur_rpn_));
    }

    Type* var_type = TYPE(); // from type to name
    while (tokenName() != ";") {
        std::string var_name = tokenName();
        if (var_nums_.find(var_name) == var_nums_.end()) {
            int var_size = var_nums_.size();
            var_nums_[var_name] = var_size;
        }
        cur_tid_->setVar(var_nums_[var_name]);
        pushRpn(new RPN::DeclareVar(var_nums_[var_name], var_type));
        tokenNext(); // from name to smth
        if (tokenName() == "=") {
            tokenPrev(); // from = to name
            EXPRESSION(); // from expression to , or ;
            pushRpn(new RPN::Kill());
        }
        if (tokenName() == ",") tokenNext(); // from , to name
        if (cur_tid_->parent() == NULL) {
            prev_global_var = cur_rpn_;
            pushRpn(new RPN::Goto());
        }
    }
    tokenNext(); // from ; to smth
}

void RpnCreator::COMPOUND_OPERATOR() {
    tokenNext(); // from { to smth
    while (tokenName() != "}") {
        if (tokenName() == "{") {
            nextTid();
            COMPOUND_OPERATOR(); // from compound to smth
            nextTid();
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth
        }
    }
    tokenNext(); // from } to smth
}

void RpnCreator::NON_COMPOUND_OPERATOR() {
    if (tokenName() == "break" || tokenName() == "return" || tokenName() == "continue" || tokenName() == "print" || tokenName() == "input") {
        OPERATOR(); // from operator to smth
    }
    else if (tokenName() == "if" || tokenName() == "while" ||
        tokenName() == "do" || tokenName() == "for" || tokenName() == "when") {
        DERIVED_OPERATOR(); // from derived to smth
    }
    else if (isType(token())) {
        VARIABLE_DECLARATION(); // from var_decl to smth
    }
    else {
        EXPRESSION(); // from expression to ;
        tokenNext(); // from ; to smth
        pushRpn(new RPN::Kill());
    }
}

void RpnCreator::OPERATOR() {
    if (tokenName() == "break") {
        tokenNext(); // from break to ;
        tokenNext(); // from ; to smth
        breaks_.top().push(cur_rpn_);
        Tid* tmp = cur_tid_;
        while (!tmp->isLoop()) {
            std::set<int> vars = tmp->getVars();
            for (auto i : vars)
                if (i != 0)
                    pushRpn(new RPN::DeleteVar(i));
            tmp = tmp->parent();
        }

        pushRpn(new RPN::Goto()); // will be set, when loop is over
    }
    else if (tokenName() == "continue") {
        tokenNext(); // from continue to ;
        tokenNext(); // from ; to smth
        continues_.top().push(cur_rpn_);
        Tid* tmp = cur_tid_;
        while (!tmp->isLoop()) {
            std::set<int> vars = tmp->getVars();
            for (auto i : vars)
                if (i != 0)
                    pushRpn(new RPN::DeleteVar(i));
            tmp = tmp->parent();
        }
        pushRpn(new RPN::Goto()); // will be set, when loop is over
    }
    else if (tokenName() == "return") {
        tokenNext(); // from return to expression;



        if (tokenName() == ";") {
            tokenNext(); // from ; to smth
            pushRpn(new RPN::Const(new Data::Bool()));
        }
        else {
            EXPRESSION(); // from expression to ;
            pushRpn(new RPN::Copy());
            tokenNext(); // from ; to smth
        }

        Tid* tmp = cur_tid_;
        while (!tmp->isFunction()) {
            std::set<int> vars = tmp->getVars();
            for (auto i : vars)
                if (i != 0)
                    pushRpn(new RPN::DeleteVar(i));
            tmp = tmp->parent();
        }

        std::set<int> vars = tmp->getVars();
        for (auto i : vars)
            if (i != 0)
                pushRpn(new RPN::DeleteVar(i));

        pushRpn(new RPN::ReturnGoto()); // return address will be know in runtime
    }
    else if (tokenName() == "print") {
        tokenNext(); // from print to (

        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            EXPRESSION(); // from expression to , or )
            pushRpn(new RPN::Print());

            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
        }
        pushRpn(new RPN::Const(new Data::Char('\n')));
        pushRpn(new RPN::Print());
        tokenNext(); // from ) to ;
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "input") {
        tokenNext(); // from print to (
        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            EXPRESSION(); // from expression to , or )
            pushRpn(new RPN::Input());

            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
        }
        tokenNext(); // from ) to ;
        tokenNext(); // from ; to smth
    }
}

void RpnCreator::DERIVED_OPERATOR() {
    if (tokenName() == "if") {
        IF();
    }
    else if (tokenName() == "while") {
        WHILE();
    }
    else if (tokenName() == "do") {
        DO_WHILE();
    }
    else if (tokenName() == "for") {
        FOR();
    }
    else if (tokenName() == "when") {
        WHEN();
    }
}

void RpnCreator::IF() {
    tokenNext(); // from if to (

    tokenNext(); // from ( to expression

    EXPRESSION(); // from expression to { [starts (, ends )]
    pushRpn(new RPN::Cast(new Type("bool")));
    int p2 = cur_rpn_;
    pushRpn(new RPN::FalseGoto()); // will be set, when if ends


    tokenNext(); // from ) to {

    nextTid();
    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }
    prevTid();
    int p3 = cur_rpn_;
    pushRpn(new RPN::Goto()); // will be set, when else ends

    setRpn(p2, new RPN::FalseGoto(cur_rpn_));

    if (tokenName() == "else") {
        tokenNext(); // from else to compound or derived

        nextTid();
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to smth
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth;
        }
        prevTid();
    }
    setRpn(p3, new RPN::Goto(cur_rpn_));
}

void RpnCreator::WHILE() {
    nextTid(true, false);

    tokenNext(); // from while to (

    breaks_.push(std::stack<int>());
    continues_.push(std::stack<int>());

    tokenNext(); // from ( to expression
    int p1 = cur_rpn_;
    EXPRESSION(); // from expression to { [starts (, ends )]
    pushRpn(new RPN::Cast(new Type("bool")));
    int p2 = cur_rpn_;
    pushRpn(new RPN::FalseGoto()); // will be set, when while ends

    tokenNext(); // from ) to {

    nextTid();
    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth;
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth
    }
    prevTid();
    pushRpn(new RPN::Goto(p1));
    setRpn(p2, new RPN::FalseGoto(cur_rpn_));

    setBreaks();
    setContinues(p1);
    prevTid();
}

void RpnCreator::DO_WHILE() {
    nextTid(true, false);
    breaks_.push(std::stack<int>());
    continues_.push(std::stack<int>());

    int p1 = cur_rpn_;

    tokenNext(); // from do to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

    tokenNext(); // from while to (

    tokenNext(); // from ( to expression
    int p2 = cur_rpn_;
    EXPRESSION(); // from expression to ; [starts (, ends )]
    pushRpn(new RPN::Cast(new Type("bool")));

    int p3 = cur_rpn_;
    pushRpn(new RPN::FalseGoto()); // will be set, when do_while ends
    pushRpn(new RPN::Goto(p1));

    tokenNext(); // from ) to ;

    tokenNext(); // from ; to smth

    setRpn(p3, new RPN::FalseGoto(cur_rpn_));
    setBreaks();
    setContinues(p2);
    prevTid();
}

void RpnCreator::FOR() {
    nextTid(true, false);
    breaks_.push(std::stack<int>());
    continues_.push(std::stack<int>());

    tokenNext(); // from for to (

    tokenNext(); // from ( to name

    Type* var_type = TYPE(); // from type to name

    std::string var_name = tokenName();

    pushRpn(new RPN::DeclareIterator());

    if (var_nums_.find(var_name) == var_nums_.end()) {
        int var_num = var_nums_.size();
        var_nums_[var_name] = var_num;
    }

    cur_tid_->setVar(var_nums_[var_name]);


    pushRpn(new RPN::DeclareVar(var_nums_[var_name], var_type));
    tokenNext(); // from name to :

    tokenNext(); // from : to expression

    EXPRESSION(); // from expression to { (expression starts with : and ends with ))

    int p2 = cur_rpn_;
    pushRpn(new RPN::ForGoto()); // will be set, when for ends
    pushRpn(new RPN::SetIteratorVar(var_nums_[var_name]));

    tokenNext(); // from ) to {

    nextTid();
    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }
    prevTid();

    pushRpn(new RPN::MoveIterator());
    pushRpn(new RPN::Goto(p2));
    setRpn(p2, new RPN::ForGoto(cur_rpn_));
    setBreaks();
    setContinues(p2);
    pushRpn(new RPN::DeleteIterator());
    prevTid();
}

void RpnCreator::WHEN() {
    tokenNext(); // from when to (

    tokenNext(); // from ( to expression

    EXPRESSION(); // from expression to { [expression starts with ( and ends with )]

    tokenNext(); // from ) to {

    tokenNext(); // from { to expression or else

    std::vector<int> p;

    while (tokenName() != "else") {
        EXPRESSION(); // from expression to { (expression ends with : )

        pushRpn(new RPN::WhenCompare());
        int p2 = cur_rpn_;
        pushRpn(new RPN::FalseGoto()); // will be set, when branch ends


        tokenNext(); // from : to 

        nextTid();
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to expression
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to expression
        }
        prevTid();
        p.push_back(cur_rpn_);
        pushRpn(new RPN::Goto()); // will be set, when when ends
        setRpn(p2, new RPN::FalseGoto(cur_rpn_));

        if (tokenName() == "}") break;

    }
    if (tokenName() == "else") {
        tokenNext(); // from else to :

        tokenNext(); // from : to operator

        nextTid();
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to }
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to }
        }
        prevTid();
    }

    for (auto& i : p)
        setRpn(i, new RPN::Goto(cur_rpn_));

    pushRpn(new RPN::Kill());

    tokenNext(); // from } to smth
}

void RpnCreator::EXPRESSION() {
    std::stack<std::string> st;
    int balance = 0;

    for (;

        !(tokenName() == ":" || tokenName() == "," ||
            tokenName() == ";" || tokenName() == "}" || tokenName() == "]" ||
            (balance == 0 && tokenName() == ")"));

        tokenNext()) {


        if (tokenType() == LecsicalEnum::NAME) {
            tokenNext();
            if (tokenName() == "(") { // function
                tokenPrev();
                FUNCTION_CALL();
            }
            else {
                tokenPrev();
                VARIABLE();
            }
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_BOOL) {
            CONST_BOOL();
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_CHAR) {
            CONST_CHAR();
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_FLOAT) {
            CONST_FLOAT();
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_INT) {
            CONST_INT();
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_STRING) {
            CONST_STRING();
            continue;
        }

        if (tokenName() == "(") {
            balance++;
            st.push(tokenName());
            continue;
        }

        if (tokenName() == ")") {
            balance--;
            while (st.top() != "(") {
                pushRpn(new RPN::Operation(st.top()));
                st.pop();
            }
            st.pop();
            continue;
        }

        if (tokenType() == LecsicalEnum::OPERATION) {
            while (!st.empty() && st.top() != "(" && getPrior(st.top()) < getPrior(tokenName())) {
                pushRpn(new RPN::Operation(st.top()));
                st.pop();
            }
            while (!st.empty() && st.top() != "(" && getPrior(st.top()) == getPrior(tokenName()) && !isRightAssoc(tokenName())) {
                pushRpn(new RPN::Operation(st.top()));
                st.pop();
            }
            st.push(tokenName());
            continue;
        }

        if (tokenName() == ".") {
            METHOD_CALL();
            continue;
        }

        if (tokenName() == "[") {
            INDEX_CALL();
            continue;
        }

        if (tokenName() == "{") {
            INITIALIZER_LIST();
            continue;
        }

        if (isType(token())) {
            CAST();
            continue;
        }

        if (tokenName() == "it") {
            pushRpn(new RPN::Var(var_nums_[tokenName()]));
            continue;
        }
    }

    while (!st.empty()) {
        if (st.top() != "(")
            pushRpn(new RPN::Operation(st.top()));
        st.pop();
    }
}

void RpnCreator::FUNCTION_CALL() {
    FuncId func = function_calls_.front();
    function_calls_.pop();
    tokenNext(); // from name to (
    tokenNext(); // from ( to ,
    while (tokenName() != ")") {
        EXPRESSION();
        if (tokenName() == ",")
            tokenNext();
    }
    // from ) to smth does for-loop in expression
    function_calls_rpn_.push(cur_rpn_);
    pushRpn(new RPN::FuncCall(0, 0));
}

void RpnCreator::METHOD_CALL() {
    tokenNext(); // from . to smth
    MethodId method = method_calls_.front();
    method_calls_.pop();
    tokenNext(); // from name to (
    tokenNext(); // from ( to ,
    while (tokenName() != ")") {
        EXPRESSION();
        if (tokenName() == ",")
            tokenNext();
    }

    if (method.argsNum() == 0 &&
        (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
            method.methodType()->baseType() == Type::TypeEnum::STRING)
        && (method.name() == "size" || method.name() == "popBack")) {
        pushRpn(new RPN::SetIt());
    }
    else if (method.argsNum() == 1 &&
        (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
            method.methodType()->baseType() == Type::TypeEnum::STRING)
        && method.name() == "pushBack") {
        pushRpn(new RPN::SetIt());
    }

    // from ) to smth does for-loop in expression
    method_calls_rpn_.push(cur_rpn_);
    pushRpn(new RPN::FuncCall(0, 0));
    pushRpn(new RPN::DelIt());
}

void RpnCreator::INDEX_CALL() {
    tokenNext(); // from [ to expression
    EXPRESSION();
    pushRpn(new RPN::Operation("[]"));
}

void RpnCreator::VARIABLE() {
    pushRpn(new RPN::Var(var_nums_[tokenName()]));
}

void RpnCreator::CONST_BOOL() {
    bool num = tokenName() == "true";
    pushRpn(new RPN::Const(new Data::Bool(num)));
}

void RpnCreator::CONST_CHAR() {
    char num;
    if (tokenName().size() > 3) {
        std::string name = tokenName();
        name.erase(name.begin());
        name.pop_back();
        std::string shielded = "abfnrtv\"\'\\";
        for (int i = 0; i < name.size(); ++i) {
            if (name[i] == '\\') {
                char change;
                switch (name[i + 1])
                {
                case 'a':
                    change = '\a';
                    break;
                case 'b':
                    change = '\b';
                    break;
                case 'f':
                    change = '\f';
                    break;
                case 'n':
                    change = '\n';
                    break;
                case 'r':
                    change = '\r';
                    break;
                case 't':
                    change = '\t';
                    break;
                case 'v':
                    change = '\v';
                    break;
                case '\"':
                    change = '\"';
                    break;
                case '\'':
                    change = '\'';
                    break;
                case '\\':
                    change = '\\';
                    break;
                default:
                    break;
                }
                name.erase(name.begin() + i);
                name[i] = change;
            }
        }
        num = name[0];
    }
    else {
        num = tokenName()[1];
    }
    pushRpn(new RPN::Const(new Data::Char(num)));
}

void RpnCreator::CONST_FLOAT() {
    double num = std::stod(tokenName());
    pushRpn(new RPN::Const(new Data::Float(num)));
}

void RpnCreator::CONST_INT() {
    int64_t num = std::stoll(tokenName());
    pushRpn(new RPN::Const(new Data::Int(num)));
}

void RpnCreator::CONST_RANGE() {
    std::string fs, ts, ss, name;
    fs = ts = ss = "";
    name = tokenName();
    int i = 0;
    while (name[i] != '.') fs += name[i++];
    i += 2;
    while (i < name.size() && name[i] != '.') ts += name[i++];
    i += 2;
    while (i < name.size()) ss += name[i++];
    if (ss.empty()) ss = "1";
    int from = std::stoll(fs), to = std::stoll(ts), step = std::stoll(ss);
    pushRpn(new RPN::Const(new Data::Range(from, to, step)));
}

void RpnCreator::CONST_STRING() {
    std::string name = tokenName();
    name.erase(name.begin());
    name.pop_back();
    std::string shielded = "abfnrtv\"\'\\";
    for (int i = 0; i < name.size(); ++i) {
        if (name[i] == '\\') {
            char change;
            switch (name[i + 1])
            {
            case 'a':
                change = '\a';
                break;
            case 'b':
                change = '\b';
                break;
            case 'f':
                change = '\f';
                break;
            case 'n':
                change = '\n';
                break;
            case 'r':
                change = '\r';
                break;
            case 't':
                change = '\t';
                break;
            case 'v':
                change = '\v';
                break;
            case '\"':
                change = '\"';
                break;
            case '\'':
                change = '\'';
                break;
            case '\\':
                change = '\\';
                break;
            default:
                break;
            }
            name.erase(name.begin() + i);
            name[i] = change;
        }
    }
    pushRpn(new RPN::Const(new Data::String(name)));
}

void RpnCreator::INITIALIZER_LIST() {
    tokenNext(); // from { to smth
    pushRpn(new RPN::CreateArray());
    while (tokenName() != "}") {
        EXPRESSION(); // from expression to , or }
        pushRpn(new RPN::PushInit());
        if (tokenName() == ",")
            tokenNext(); // from , to smth
    }
    //tokenNext(); // from } to smth
}

void RpnCreator::CAST() {
    Type* type = TYPE();
    EXPRESSION();
    pushRpn(new RPN::Cast(type));
    tokenPrev(); // back to ), because expr-for will tokenNext later
}

int RpnCreator::getPrior(std::string op) {
    if (op == "[]") return 2;
    if (op == "**") return 3;
    if (op == "!") return 4;
    if (op == "~") return 4;
    if (op == "@") return 4;
    if (op == "_") return 4;
    if (op == "*") return 5;
    if (op == "/") return 5;
    if (op == "%") return 5;
    if (op == "+") return 6;
    if (op == "-") return 6;
    if (op == "<<") return 7;
    if (op == ">>") return 7;
    if (op == "<") return 8;
    if (op == ">") return 8;
    if (op == "<=") return 8;
    if (op == ">=") return 8;
    if (op == "==") return 9;
    if (op == "!=") return 9;
    if (op == "&") return 10;
    if (op == "^") return 11;
    if (op == "|") return 12;
    if (op == "..") return 13;
    if (op == "in") return 14;
    if (op == "&&") return 15;
    if (op == "||") return 16;
    return 16;
}

bool RpnCreator::isRightAssoc(std::string op) {
    for (auto& i : { "!", "~", "@", "_", "**", })
        if (op == i)
            return true;
    return false;
}



void RpnCreator::setBreaks() {
    while (!breaks_.top().empty()) {
        setRpn(breaks_.top().top(), new RPN::Goto(cur_rpn_));
        breaks_.top().pop();
    }
    breaks_.pop();
}

void RpnCreator::setContinues(int address) {
    while (!continues_.top().empty()) {
        setRpn(breaks_.top().top(), new RPN::Goto(address));
        continues_.top().pop();
    }
    continues_.pop();
}

void RpnCreator::setCalls() {
    while (!fc_copy.empty()) {
        FuncId func = fc_copy.front();
        int address = function_calls_rpn_.front();
        setRpn(address, new RPN::FuncCall(funcs_[func], func.argsNum()));
        fc_copy.pop();
        function_calls_rpn_.pop();
    }

    while (!mc_copy.empty()) {
        MethodId method = mc_copy.front();
        int address = method_calls_rpn_.front();
        if (method.argsNum() == 0 &&
            (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
                method.methodType()->baseType() == Type::TypeEnum::STRING)) {
            if (method.name() == "size") {
                setRpn(address, new RPN::SizeArray());
            }
            else if (method.name() == "popBack") {
                setRpn(address, new RPN::PopArray());
            }
            else {
                setRpn(address, new RPN::FuncCall(methods_[method], method.argsNum()));
            }
        }
        else if (method.argsNum() == 1 &&
            (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
                method.methodType()->baseType() == Type::TypeEnum::STRING)) {
            if (method.name() == "pushBack") {
                setRpn(address, new RPN::PushArray());
            }
            else {
                setRpn(address, new RPN::FuncCall(methods_[method], method.argsNum()));
            }
        }
        else {
            setRpn(address, new RPN::FuncCall(methods_[method], method.argsNum()));
        }
        mc_copy.pop();
        method_calls_rpn_.pop();
    }
}

void RpnCreator::debug() {
    int i = 0;
    for (auto t : rpn_) {
        std::cout << i++ << ": ";
        t->show();
    }
}