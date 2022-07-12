#include "SemanticsAnalyzer.h"

class SemanticsAnalyzer::SemanticsError {
private:
    std::string error_;
    LecsicalToken token_;
public:
    SemanticsError(std::string error, LecsicalToken token) : error_(error), token_(token) {}
    void sendError() {
        std::cout << "File: " << token_.file_name() << "\n";
        std::cout << "Line: " << token_.lineNumber() << ", character: " << token_.charNumber() << ".\n";
        std::cout << "Semantical error: " << error_;
        std::cout << '\n';
    }
};

class SemanticsAnalyzer::TID {
private:
    TID* parent_;
    std::map<std::string, Type*> vars_;
public:
    TID(TID* parent = NULL) : parent_(parent) {}
    TID* parent() { return parent_; }
    Type* varGet(std::string name) {
        if (parent_ == NULL) {
            if (vars_.find(name) == vars_.end()) return new Type(Type::TypeEnum::VOID);
            return vars_[name];
        }
        if (vars_.find(name) == vars_.end()) return parent_->varGet(name);
        return vars_[name];
    }

    bool varExistLocal(std::string name) {
        return vars_.find(name) != vars_.end();
    }

    bool varExist(std::string name) {
        if (vars_.find(name) != vars_.end()) return true;
        if (parent_ != NULL) return parent_->varExist(name);
        return false;
    }

    void varSet(std::string name, Type* type) {
        vars_[name] = type;
    }
};



SemanticsAnalyzer::SemanticsAnalyzer(std::vector<LecsicalToken> tokens) :
    tokens_(tokens), cur_token_(0), analysis_stage_(0), tid_(new TID()) {
    cur_func_type_ = NULL;
    returns_cnt_ = 0;
    loop_cnt_ = 0;
    in_method_ = false;
}


std::queue<MethodId> SemanticsAnalyzer::getMethodCalls() { return method_calls_; }
std::queue<FuncId> SemanticsAnalyzer::getFunctionCalls() { return function_calls_; }


LecsicalToken SemanticsAnalyzer::token() { return tokens_[cur_token_]; }
std::string SemanticsAnalyzer::tokenName() { return tokens_[cur_token_].name(); }
LecsicalEnum SemanticsAnalyzer::tokenType() { return tokens_[cur_token_].type(); }
void SemanticsAnalyzer::tokenNext() { cur_token_++; }
void SemanticsAnalyzer::tokenPrev() { cur_token_--; }
std::vector<LecsicalToken> SemanticsAnalyzer::getTokens() { return tokens_; }
std::string SemanticsAnalyzer::tokenFile() { return tokens_[cur_token_].file_name(); }
int SemanticsAnalyzer::tokenLine() { return tokens_[cur_token_].lineNumber(); }
int SemanticsAnalyzer::tokenChar() { return tokens_[cur_token_].charNumber(); }

void SemanticsAnalyzer::nextTID() { tid_ = new TID(tid_); }
void SemanticsAnalyzer::prevTID() { tid_ = tid_->parent(); }

bool SemanticsAnalyzer::isType(LecsicalToken token) {
    return token.name() == "bool" || token.name() == "char" ||
        token.name() == "int" || token.name() == "float" || token.name() == "string" || token.name() == "range" || token.name() == "array";
}

bool SemanticsAnalyzer::goodCast(Type* first, Type* second) {
    // first - cast to, second - cast from
    while (first->parent() != NULL && second->parent() != NULL) {
        first = first->parent();
        second = second->parent();
    }
    if (first->parent() == NULL && second->parent() == NULL) {
        int tBool = int(Type::TypeEnum::BOOL),
            tChar = int(Type::TypeEnum::CHAR),
            tInt = int(Type::TypeEnum::INT),
            tFloat = int(Type::TypeEnum::FLOAT),
            tString = int(Type::TypeEnum::STRING),
            tRange = int(Type::TypeEnum::RANGE);
        std::map<int, std::vector<int>> casts = {
            {tBool, {tBool, tChar, tInt, tFloat, tString}},
            {tChar, {tBool, tChar, tInt, tFloat, tString}},
            {tInt, {tBool, tChar, tInt, tFloat, tString}},
            {tFloat, {tBool, tChar, tInt, tFloat, tString}},
            {tString, {tBool, tChar, tInt, tFloat, tString}},
            {tRange, {}}
        };

        return std::find(casts[int(first->baseType())].begin(), casts[int(first->baseType())].end(), int(second->baseType())) != casts[int(first->baseType())].end();

    }
    return false;
}

void SemanticsAnalyzer::work() {
    try {
        cur_token_ = 0;
        returns_cnt_ = 0;
        loop_cnt_ = 0;
        in_method_ = false;
        if (tokens_.size() == 0) return;
        if (analysis_stage_ == 2) return;
        if (analysis_stage_ == 0)
            tokens_.push_back(LecsicalToken("", LecsicalEnum::NONE, (tokens_.end() - 1)->lineNumber(), (tokens_.end() - 1)->charNumber(), ""));

        while (tokenType() != LecsicalEnum::NONE) {
            int start = cur_token_;

            if (tokenName() == "void") {
                tokenNext(); // from void to smth
                if (isType(token())) {
                    cur_token_ = start;
                    METHOD_DECLARATION();
                }
                else {
                    cur_token_ = start;
                    FUNCTION_DECLARATION();
                }
                continue;
            }

            TYPE();

            if (isType(token())) {
                cur_token_ = start;
                METHOD_DECLARATION();
            }
            else if (tokenType() == LecsicalEnum::NAME) {
                tokenNext();
                if (tokenName() == "(") {
                    cur_token_ = start;
                    FUNCTION_DECLARATION();
                }
                else {
                    cur_token_ = start;
                    VARIABLE_DECLARATION();
                }
            }
        }
        analysis_stage_++;
        work();
        return;

    }
    catch (SemanticsError error) {
        error.sendError();
        throw std::string("");
    }
}

Type* SemanticsAnalyzer::TYPE() {
    if (isType(token()) && tokenName() != "array") {
        std::string type = tokenName();
        tokenNext(); // from basic types to smth
        return new Type(type);
    }
    else if (tokenName() == "array") {

        // array

        tokenNext(); // from array to <

        tokenNext(); // from < to TYPE

        Type* base = TYPE(); // from type to >

        tokenNext(); // from > to smth

        return new Type(Type::TypeEnum::ARRAY, base);
    }
}

void SemanticsAnalyzer::FUNCTION_DECLARATION() {
    if (analysis_stage_ == 1)
        nextTID();

    Type* func_type;
    if (tokenName() != "void") {
        func_type = TYPE(); // from TYPE to name
    }
    else {
        func_type = new Type(Type::TypeEnum::VOID);
        tokenNext(); // from void to name
    }
    cur_func_type_ = func_type;
    returns_cnt_ = 0;
    std::string func_name = tokenName();
    tokenNext(); // from name to (
    tokenNext(); // from ( to smth

    if (func_name == "main" && *func_type != Type())
        throw SemanticsError("Main function must be void.", token());

    std::vector<Type*> func_args;

    bool has_default = false;

    if (tokenName() != ")") {
        while (true) {

            std::pair<Type*, bool> arg_type = FUNCTION_ARGUMENT(); // from func_arg to , or )
            if (has_default && !arg_type.second)
                throw SemanticsError("Argument must have deafult value if previous argument has one.", token());

            has_default = has_default || arg_type.second;

            if (has_default && analysis_stage_ == 0) {
                FuncId func(func_name, func_args);

                if (funcs_.find(func) != funcs_.end()) {
                    std::string error = "Redeclarartion of function. Name: " + func_name + ", arguments: ";
                    for (auto& arg : func_args) {
                        error += arg->toString();
                        error += ", ";
                    }
                    error += ".";
                    throw SemanticsError(error, token());
                }

                funcs_[func] = func_type;
            }

            func_args.push_back(arg_type.first);

            if (tokenName() == ",") {
                tokenNext(); // from , to func_arg
            }
            else if (tokenName() == ")") {
                break;
            }
        }
    }

    if (func_name == "main" && func_args.size() != 0)
        throw SemanticsError("Main function does not take arguments", token());

    tokenNext(); // from ) to {
    if (analysis_stage_ == 0) {
        FuncId func(func_name, func_args);

        if (funcs_.find(func) != funcs_.end()) {
            std::string error = "Redeclarartion of function. Name: " + func_name + ", arguments: ";
            for (auto& arg : func_args) {
                error += arg->toString();
                error += ", ";
            }
            error += ".";
            throw SemanticsError(error, token());
        }

        funcs_[func] = func_type;
    }
    if (analysis_stage_ == 1)
        nextTID();

    COMPOUND_OPERATOR(); // from compound to smth

    if (analysis_stage_ == 1)
        prevTID();
    if (analysis_stage_ == 1)
        prevTID();

    if (*cur_func_type_ != Type(Type::TypeEnum::VOID) && returns_cnt_ == 0)
        throw SemanticsError("Non-void-function must return something.", token());

    cur_func_type_ = NULL;
}

void SemanticsAnalyzer::METHOD_DECLARATION() {
    if (analysis_stage_ == 1)
        nextTID();

    in_method_ = true;
    Type* method_type;
    if (tokenName() != "void") {
        method_type = TYPE(); // from type to type
    }
    else {
        method_type = new Type(Type::TypeEnum::VOID);
        tokenNext();
    }
    Type* method_name_type = TYPE(); // from type to name
    cur_func_type_ = method_type;
    returns_cnt_ = 0;
    tokenNext(); // from . to name
    std::string method_name = tokenName();
    tokenNext(); // from name to (
    tokenNext(); // from ( to smth

    if (analysis_stage_ == 1)
        tid_->varSet("it", method_name_type);

    std::vector<Type*> method_args;

    bool predeclared = false;

    if (tokenName() != ")") {
        while (true) {
            std::pair<Type*, bool> arg_type = FUNCTION_ARGUMENT(); // from func_arg to , or )
            predeclared = predeclared || arg_type.second;

            if (predeclared && analysis_stage_ == 0) {

                if (method_name_type->baseType() == Type::TypeEnum::ARRAY &&
                    (method_name == "size" || method_name == "pushBack" || method_name == "popBack") &&
                    method_args.size() == 0)
                    throw SemanticsError("Redeclaration of built-in methods.", token());

                MethodId method(method_name_type, method_name, method_args);
                if (methods_.find(method) != methods_.end()) {
                    std::string error = "Redeclarartion of method. Name: "
                        + method_name_type->toString() + "." + method_name + ", arguments: ";
                    for (auto& arg : method_args) {
                        error += arg->toString();
                        error += ", ";
                    }
                    error += ".";
                    throw SemanticsError(error, token());
                }

                methods_[method] = method_type;
            }

            method_args.push_back(arg_type.first);

            if (tokenName() == ",") {
                tokenNext(); // from , to func_arg
            }
            else if (tokenName() == ")") {
                break;
            }
        }
    }
    tokenNext(); // from ) to {

    if (analysis_stage_ == 0) {
        if (method_name_type->baseType() == Type::TypeEnum::ARRAY &&
            (method_name == "size" || method_name == "pushBack" || method_name == "popBack") &&
            method_args.size() == 0)
            throw SemanticsError("Redeclaration of built-in methods.", token());

        MethodId method(method_name_type, method_name, method_args);
        if (methods_.find(method) != methods_.end()) {
            std::string error = "Redeclarartion of method. Name: "
                + method_name_type->toString() + "." + method_name + ", arguments: ";
            for (auto& arg : method_args) {
                error += arg->toString();
                error += ", ";
            }
            error += ".";
            throw SemanticsError(error, token());
        }

        methods_[method] = method_type;
    }
    if (analysis_stage_ == 1)
        nextTID();

    COMPOUND_OPERATOR(); // from compound to smth

    if (analysis_stage_ == 1)
        prevTID();
    if (analysis_stage_ == 1)
        prevTID();

    if (*cur_func_type_ != Type(Type::TypeEnum::VOID) && returns_cnt_ == 0)
        throw SemanticsError("Non-void-function must return something.", token());

    cur_func_type_ = NULL;
    in_method_ = false;
}

void SemanticsAnalyzer::VARIABLE_DECLARATION() {
    Type* var_type = TYPE(); // from type to name
    var_type->setIsVar(true);

    while (true) {
        std::string var_name = tokenName();
        if (analysis_stage_ == 1 && tid_->varExistLocal(var_name))
            throw SemanticsError("Redeclaration of variable. Name: " + var_name + ".", token());

        tokenNext(); // from name to = or , or ;
        if (tokenName() == "=") {
            tokenNext(); // from = to expression
            Type* expr_type = EXPRESSION(); // from expression to ; or ,

            if (analysis_stage_ == 1) {
                std::string error = "Can not = with " + var_type->toString() + " and " + expr_type->toString() + ".";

                if (*var_type < Type("string") && *expr_type < Type("string"));
                else if (*var_type == *expr_type);
                else throw SemanticsError(error, token());

            }
        }
        if (analysis_stage_ == 1)
            tid_->varSet(var_name, var_type);

        if (tokenName() == ";") {
            break;
        }
        else if (tokenName() == ",") {
            tokenNext(); // from , to name
            continue;
        }
    }
    tokenNext(); //from ; to smth
}

std::pair<Type*, bool> SemanticsAnalyzer::FUNCTION_ARGUMENT() {
    Type* arg_type = TYPE(); // from type to name
    arg_type->setIsVar(true);
    std::string arg_name = tokenName();
    if (analysis_stage_ == 1 && tid_->varExistLocal(arg_name))
        throw SemanticsError("Redeclaration of variable: " + arg_name + ".", token());

    if (analysis_stage_ == 1) {
        tid_->varSet(arg_name, arg_type);
    }

    tokenNext(); // from name to smth
    bool predeclared = false;
    if (tokenName() == "=") {
        predeclared = true;
        tokenPrev(); // from = to expression
        Type* expr_type = EXPRESSION(); // from expression to smth

        if (analysis_stage_ == 1 && *arg_type != *expr_type)
            throw SemanticsError("Different type of variable ( " +
                arg_type->toString() + " ) and expression ( " + expr_type->toString() + " ).", token());
    }
    return { arg_type, predeclared };
}


// OPERATORS

void SemanticsAnalyzer::COMPOUND_OPERATOR() {
    tokenNext();

    while (tokenName() != "}") {
        if (tokenName() == "{") {
            if (analysis_stage_ == 1)
                nextTID();
            COMPOUND_OPERATOR(); // from compound to smth

            if (analysis_stage_ == 1)
                prevTID();
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth
        }
    }

    tokenNext(); // from } to smth
}

void SemanticsAnalyzer::NON_COMPOUND_OPERATOR() {
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
    }
}

void SemanticsAnalyzer::OPERATOR() {
    if (tokenName() == "break") {
        if (analysis_stage_ == 1 && loop_cnt_ == 0) {
            throw SemanticsError("Can not use break outside loops.", token());
        }
        tokenNext(); // from break to ;
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "continue") {
        if (analysis_stage_ == 1 && loop_cnt_ == 0) {
            throw SemanticsError("Can not use continue outside loops.", token());
        }
        tokenNext(); // from continue to ;
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "return") {
        tokenNext(); // from return to expression;
        returns_cnt_++;
        if (tokenName() == ";") {
            if (analysis_stage_ == 1 && *cur_func_type_ != Type(Type::TypeEnum::VOID))
                throw SemanticsError("Non-void-function must return something", token());
            tokenNext(); // from ; to smth
        }
        else {
            Type* expr_type = EXPRESSION(); // from expression to ;
            tokenNext(); // from ; to smth

            if (analysis_stage_ == 1 && *cur_func_type_ == Type(Type::TypeEnum::VOID))
                throw SemanticsError("Void-function can not return expression.", token());
            if (analysis_stage_ == 1 && *cur_func_type_ != *expr_type)
                throw SemanticsError("Wrong returning type. Expected: " + cur_func_type_->toString() +
                    ". Got: " + expr_type->toString() + ".", token());
        }
    }
    else if (tokenName() == "print") {
        tokenNext(); // from print to (

        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            Type* expr_type = EXPRESSION(); // from expression to , or )
            if (analysis_stage_ == 1 && *expr_type == Type("void"))
                throw SemanticsError("Can not print void value", token());
            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
        }
        tokenNext(); // from ) to ;
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "input") {
        tokenNext(); // from print to (
        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            Type* expr_type = EXPRESSION(); // from expression to , or )

            if (analysis_stage_ == 1 && !expr_type->isVar())
                throw SemanticsError("Can not use input with rvalue", token());

            if (analysis_stage_ == 1 &&
                (expr_type->baseType() == Type::TypeEnum::ARRAY || expr_type->baseType() == Type::TypeEnum::RANGE))
                throw SemanticsError("Can not use input with array or range.", token());

            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
        }
        tokenNext(); // from ) to ;
        tokenNext(); // from ; to smth
    }
}

void SemanticsAnalyzer::DERIVED_OPERATOR() {
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

void SemanticsAnalyzer::IF() {

    tokenNext(); // from if to (

    tokenNext(); // from ( to expression

    Type* expr_type = EXPRESSION(); // from expression to )

    if (analysis_stage_ == 1 && !(*expr_type < Type("string")))
        throw SemanticsError("Wrong expression type. Expected: " + Type(Type::TypeEnum::BOOL).toString() +
            ". Got: " + expr_type->toString() + ".", token());

    tokenNext(); // from ) to {

    if (analysis_stage_ == 1)
        nextTID();

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

    if (analysis_stage_ == 1)
        prevTID();

    if (tokenName() == "else") {
        tokenNext(); // from else to compound or derived

        if (analysis_stage_ == 1)
            nextTID();

        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to smth
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth;
        }

        if (analysis_stage_ == 1)
            prevTID();
    }
}

void SemanticsAnalyzer::WHILE() {
    tokenNext(); // from while to (

    tokenNext(); // from ( to expression

    Type* expr_type = EXPRESSION(); // from expression to )

    if (analysis_stage_ == 1 && !(*expr_type < Type("string")))
        throw SemanticsError("Wrong expression type. Expected: " + Type(Type::TypeEnum::BOOL).toString() +
            ". Got: " + expr_type->toString() + ".", token());

    tokenNext(); // from ) to {

    if (analysis_stage_ == 1)
        nextTID();
    loop_cnt_++;
    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth;
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth
    }
    if (analysis_stage_ == 1)
        prevTID();
    loop_cnt_--;
}

void SemanticsAnalyzer::DO_WHILE() {
    if (analysis_stage_ == 1)
        nextTID();
    loop_cnt_++;

    tokenNext(); // from do to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

    tokenNext(); // from while to (

    tokenNext(); // from ( to expression

    Type* expr_type = EXPRESSION();
    if (analysis_stage_ == 1 && !(*expr_type < Type("string")))
        throw SemanticsError("Wrong expression type. Expected: " + Type(Type::TypeEnum::BOOL).toString() +
            ". Got: " + expr_type->toString() + ".", token());

    tokenNext(); // from ) to ;

    tokenNext(); // from ; to smth

    if (analysis_stage_ == 1)
        prevTID();
    loop_cnt_--;
}

void SemanticsAnalyzer::FOR() {
    if (analysis_stage_ == 1)
        nextTID();
    loop_cnt_++;

    tokenNext(); // from for to (

    tokenNext(); // from ( to name

    Type* var_type = TYPE(); // from type to name

    std::string var_name = tokenName();

    if (analysis_stage_ == 1) {
        var_type->setIsVar(true);
        tid_->varSet(var_name, var_type);
    }

    tokenNext(); // from name to :

    tokenNext(); // from : to expression

    Type* expr_type = EXPRESSION(); // from expression to )

    if (analysis_stage_ == 1) {

        if (!(*var_type == Type("int") && *expr_type == Type("range"))
            && !(*var_type == Type("char") && *expr_type == Type("string")) &&
            (expr_type->parent() == NULL || *var_type != expr_type->parent()))
            throw SemanticsError("Can not iterate with " + var_type->toString() + " by " + expr_type->toString() + ".", token());
    }

    tokenNext(); // from ) to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }
    if (analysis_stage_ == 1)
        prevTID();
    loop_cnt_--;
}

void SemanticsAnalyzer::WHEN() {
    if (analysis_stage_ == 1)
        nextTID();

    tokenNext(); // from when to (

    tokenNext(); // from ( to expression

    Type* arg_type = EXPRESSION(); // from expression to )

    tokenNext(); // from ) to {

    tokenNext(); // from { to expression or else

    while (tokenName() != "else") {
        Type* expr_type = EXPRESSION(); // from expression to :
        if (analysis_stage_ == 1 && *arg_type != *expr_type)
            throw SemanticsError("Different types of when-argument and expression. Expected: " + arg_type->toString()
                + ". Got: " + expr_type->toString() + ".", token());

        tokenNext(); // from : to 

        if (analysis_stage_ == 1)
            nextTID();
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to expression
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to expression
        }
        if (analysis_stage_ == 1)
            prevTID();

        if (tokenName() == "}") break;

    }

    if (tokenName() == "else") {
        tokenNext(); // from else to :

        tokenNext(); // from : to operator
        if (analysis_stage_ == 1)
            nextTID();
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to }
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to }
        }
        if (analysis_stage_ == 1)
            prevTID();
    }

    tokenNext(); // from } to smth

    if (analysis_stage_ == 1)
        prevTID();
}


// EXPRESSION
/*
Type* SemanticsAnalyzer::EXPRESSION() {
    if (analysis_stage_ == 0) {
        PRIORITY_16();
        return new Type();
    }
    return PRIORITY_16();
}

Type* SemanticsAnalyzer::VALUE() {

    if (tokenType() == LecsicalEnum::CONST_BOOL || tokenType() == LecsicalEnum::CONST_CHAR ||
        tokenType() == LecsicalEnum::CONST_INT || tokenType() == LecsicalEnum::CONST_FLOAT ||
        tokenType() == LecsicalEnum::CONST_STRING || tokenType() == LecsicalEnum::CONST_RANGE) {
        LecsicalEnum token_type = tokenType();
        tokenNext(); // from const to smth
        if (analysis_stage_ == 0) return new Type();
        switch (token_type)
        {
        case LecsicalEnum::CONST_BOOL:
            return new Type(Type::TypeEnum::BOOL);
            break;
        case LecsicalEnum::CONST_CHAR:
            return new Type(Type::TypeEnum::CHAR);
            break;
        case LecsicalEnum::CONST_INT:
            return new Type(Type::TypeEnum::INT);
            break;
        case LecsicalEnum::CONST_FLOAT:
            return new Type(Type::TypeEnum::FLOAT);
            break;
        case LecsicalEnum::CONST_STRING:
            return new Type(Type::TypeEnum::STRING);
            break;
        case LecsicalEnum::CONST_RANGE:
            return new Type(Type::TypeEnum::RANGE);
            break;
        default:
            return new Type(Type::TypeEnum::VOID);
            break;
        }
    }

    if (isType(token())) {
        if (analysis_stage_ == 0) {
            CAST();
            return new Type();
        }
        return CAST();
    }

    if (tokenName() == "{") {
        if (analysis_stage_ == 0) {
            INITIALIZER_LIST();
            return new Type();
        }
        return INITIALIZER_LIST(); // from init_list to smth
    }


    if (tokenName() == "it") {
        if (analysis_stage_ == 1 && in_method_ == false)
            throw SemanticsError("Can not use \"it\" not in method declaration.", token());

        tokenNext(); // from it to smth

        if (analysis_stage_ == 0)
            return new Type();
        return tid_->varGet("it");
    }
    else {
        std::string var_name = tokenName();
        tokenNext(); // from name to smth
        if (tokenName() == "(") { // function
            tokenPrev(); // from ( to name
            if (analysis_stage_ == 0) {
                FUNCTION_CALL();
                return new Type();
            }
            return FUNCTION_CALL(); // from function_call to smth
        }
        else { // variable
            if (analysis_stage_ == 1 && !tid_->varExist(var_name))
                throw SemanticsError("Undeclared variable: " + var_name, token());
            if (analysis_stage_ == 0)
                return new Type();
            return tid_->varGet(var_name);
        }
    }
}

Type* SemanticsAnalyzer::CAST() {
    Type* cast_type = TYPE(); // from type to (

    tokenNext(); // from ( to expression;

    Type* expr_type = EXPRESSION(); // from expression to )

    if (analysis_stage_ == 1 && !goodCast(cast_type, expr_type))
        throw SemanticsError("Can not cast " + expr_type->toString() + " to " + cast_type->toString() + ".", token());

    tokenNext(); // from ) to smth

    if (analysis_stage_ == 0) {
        return new Type();
    }

    return cast_type;
}

Type* SemanticsAnalyzer::INITIALIZER_LIST() {

    std::vector<Type*> types;

    while (tokenName() != "}") {
        tokenNext(); // from { or , to expression
        types.push_back(EXPRESSION()); // from expression to , or }
        if (tokenName() == "}") break;
    }
    tokenNext(); // from } to smth

    if (analysis_stage_ == 0) {
        return new Type();
    }

    if (types.size() == 0)
        return new Type(Type::TypeEnum::ARRAY, new Type(Type::TypeEnum::VOID));

    for (int i = 1; i < types.size(); ++i) {
        if (analysis_stage_ == 1 && *types[i] != *types[i - 1])
            throw SemanticsError("Different types of members of initialyzer_list.", token());
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }

    return new Type(Type::TypeEnum::ARRAY, types[0]);
}

/*
Type* SemanticsAnalyzer::METHOD_CALL() {
    std::string var_name = tokenName();
    tokenNext(); // from name to .
    tokenNext(); // from . to name
    std::string method_name = tokenName();
    tokenNext(); // from name to (

    Type* var_type;
    if (analysis_stage_ == 1)
        var_type = tid_->varGet(var_name);
    else
        var_type = new Type();

    std::vector<Type*> args;
    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        Type* expr_type = EXPRESSION(); // from expression to , or )
        args.push_back(expr_type);
        if (tokenName() == ")") break;
    }

    MethodId method(var_type, method_name, args);
    if (analysis_stage_ == 1 && methods_.find(method) == methods_.end()) {
        std::string error = "Undeclared method: ";
        error += var_type->toString();
        error += ".";
        error += method_name;
        error += "(";
        for (int i = 0; i < args.size(); ++i) {
            error += args[i]->toString();
            if (i != args.size() - 1)
                error += ", ";
        }
        error += ")";
        throw SemanticsError(error, token());
    }
    tokenNext(); // from ) to smth

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return methods_[method];
}


Type* SemanticsAnalyzer::METHOD_CALL(Type* var_type) {
    tokenNext(); // from . to name
    std::string method_name = tokenName();
    tokenNext(); // from name to (

    std::vector<Type*> args;
    Type* one_arg = NULL;
    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        Type* expr_type = EXPRESSION(); // from expression to , or )
        one_arg = expr_type;
        args.push_back(expr_type);
        if (tokenName() == ")") break;
    }

    MethodId method(var_type, method_name, args);
    if (analysis_stage_ == 1)
        method_calls_.push(method);

    if (analysis_stage_ == 1 && (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
        method.methodType()->baseType() == Type::TypeEnum::STRING) &&
        (method_name == "size" || method_name == "popBack") &&
        method.argsNum() == 0) {
        tokenNext(); // from ) to smth
        if (method_name == "size")
            return new Type("int");
        return new Type();
    }

    if (analysis_stage_ == 1 && (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
        method.methodType()->baseType() == Type::TypeEnum::STRING) &&
        method_name == "pushBack" &&
        method.argsNum() == 1) {
        tokenNext(); // from ) to smth


        Type* m_type = method.methodType();
        if (m_type->baseType() == Type::TypeEnum::STRING)
            m_type = new Type("char");
        else
            m_type = m_type->parent();


        if (*m_type != *one_arg)
            throw SemanticsError("Can not pushBack " + one_arg->toString() + " to " + method.methodType()->toString() + ".", token());

        return new Type();
    }

    if (analysis_stage_ == 1 && methods_.find(method) == methods_.end()) {

        std::string error = "Undeclared method: ";
        error += var_type->toString();
        error += ".";
        error += method_name;
        error += "(";
        for (int i = 0; i < args.size(); ++i) {
            error += args[i]->toString();
            if (i != args.size() - 1)
                error += ", ";
        }
        error += ")";
        throw SemanticsError(error, token());
    }
    tokenNext(); // from ) to smth

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return methods_[method];
}

Type* SemanticsAnalyzer::FUNCTION_CALL() {
    std::string func_name = tokenName();
    tokenNext(); // from name to (
    std::vector<Type*> args;

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        Type* expr_type = EXPRESSION(); // from expression to , or )
        args.push_back(expr_type);
        if (tokenName() == ")") break;
    }

    FuncId func(func_name, args);

    if (analysis_stage_ == 1)
        function_calls_.push(func);


    if (analysis_stage_ == 1 && funcs_.find(func) == funcs_.end()) {
        std::string error = "Undeclared function: ";
        error += func_name;
        error += "(";
        for (int i = 0; i < args.size(); ++i) {
            error += args[i]->toString();
            if (i != args.size() - 1)
                error += ", ";
        }
        error += ")";

        throw SemanticsError(error, token());
    }
    tokenNext(); // from ) to smth
    if (analysis_stage_ == 0) {
        return new Type();
    }
    Type* ret = funcs_[func];
    return funcs_[func];
}

Type* SemanticsAnalyzer::PRIORITY_1() {
    if (tokenName() == "(") {
        tokenNext(); // from ( to expression
        Type* expr_type = EXPRESSION(); // from expression to )
        tokenNext(); // from ) to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return expr_type;
    }
    if (analysis_stage_ == 0) {
        VALUE();
        return new Type();
    }
    return VALUE();
}

Type* SemanticsAnalyzer::PRIORITY_2() {
    Type* left = PRIORITY_1(); // from p1 to smth
    while (tokenName() == "[" || tokenName() == ".") {
        if (tokenName() == "[") {
            tokenNext(); // from [ to expression

            Type* right = EXPRESSION(); // from expression to ]

            tokenNext(); // from ] to smth
            if (analysis_stage_ == 1) {
                if (left->baseType() != Type::TypeEnum::ARRAY && left->baseType() != Type::TypeEnum::STRING)
                    throw SemanticsError("Can not indexate with type: " + left->toString() + ".", token());
                if (*right != Type("int"))
                    throw SemanticsError("[] operator can get only int expression.", token());
                if (left->isVar() && left->parent() != NULL) left->parent()->setIsVar(true);
                bool lvalue = true;
                if (left->baseType() == Type::TypeEnum::STRING) {
                    lvalue = false;
                    left = new Type("char");
                    left->setIsVar(lvalue);
                }
                else {
                    left = left->parent();
                    left->setIsVar(lvalue);
                }
            }
        }
        else if (tokenName() == ".") {
            if (analysis_stage_ == 0) {
                METHOD_CALL(new Type());
                return new Type();
            }
            left = METHOD_CALL(left);
        }
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_3() {
    Type* left = PRIORITY_2(); // from p2 to smth

    if (tokenName() == "**") {
        std::string operation = tokenName();
        tokenNext(); // from ** to p3
        Type* right = PRIORITY_3(); // from p3 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }

        Type* ret = operate(left, operation, right);
        return ret;
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_4() {
    if (tokenName() == "!" || tokenName() == "~" || tokenName() == "@" || tokenName() == "_") {
        std::string operation = tokenName();
        tokenNext(); // from !~+- to p4
        Type* left = PRIORITY_4(); // from p4 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation);
    }
    else {
        return PRIORITY_3(); // from p3 to smth
    }
}

Type* SemanticsAnalyzer::PRIORITY_5() {
    Type* left = PRIORITY_4(); // from p4 to smth

    if (tokenName() == "*" || tokenName() == "/" || tokenName() == "%") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p5
        Type* right = PRIORITY_5(); // from p5 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_6() {
    Type* left = PRIORITY_5(); // from p5 to smth

    if (tokenName() == "+" || tokenName() == "-") {
        std::string operation = tokenName();
        tokenNext(); // from +- to p6
        Type* right = PRIORITY_6();
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_7() {
    Type* left = PRIORITY_6(); // from p6 to smth

    if (tokenName() == "<<" || tokenName() == ">>") {
        std::string operation = tokenName();
        tokenNext(); // from oper to smth
        Type* right = PRIORITY_7();
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_8() {
    Type* left = PRIORITY_7(); // from p7 to smth

    if (tokenName() == "<" || tokenName() == ">" || tokenName() == "<=" || tokenName() == ">=") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p8
        Type* right = PRIORITY_8(); // from p8 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_9() {
    Type* left = PRIORITY_8(); // from p8 to smth

    if (tokenName() == "==" || tokenName() == "!=") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p9
        Type* right = PRIORITY_9(); // from p9 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_10() {
    Type* left = PRIORITY_9(); // from p9 to smth

    if (tokenName() == "&") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p10
        Type* right = PRIORITY_10(); // from p9 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }
    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_11() {
    Type* left = PRIORITY_10(); // from p10 to smth

    if (tokenName() == "^") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p11
        Type* right = PRIORITY_11(); // from p11 to smth
        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_12() {
    Type* left = PRIORITY_11(); // from p11 to smth

    if (tokenName() == "|") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p12
        Type* right = PRIORITY_12(); // from p12 to smth

        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}


Type* SemanticsAnalyzer::PRIORITY_13() {
    Type* left = PRIORITY_12(); // from p13 to smth


    if (tokenName() == "in") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p13
        Type* right = PRIORITY_13(); // from p13 to smth

        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_14() {
    Type* left = PRIORITY_13(); // from p13 to smth

    if (tokenName() == "&&") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p14
        Type* right = PRIORITY_14(); // from p14 to smth

        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_15() {
    Type* left = PRIORITY_14(); // from p14 to smth

    if (tokenName() == "||") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p15
        Type* right = PRIORITY_15(); // from p15 to smth

        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::PRIORITY_16() {
    Type* left = PRIORITY_15(); // from p15 to smth

    if (tokenName() == "=" || tokenName() == "+=" || tokenName() == "-=" ||
        tokenName() == "*=" || tokenName() == "/=" || tokenName() == "%=" ||
        tokenName() == "**" || tokenName() == "<<=" || tokenName() == ">>=" ||
        tokenName() == "&=" || tokenName() == "|=" || tokenName() == "^=") {
        std::string operation = tokenName();
        tokenNext(); // from oper to p16
        Type* right = PRIORITY_16(); // from p16 to smth

        if (analysis_stage_ == 0) {
            return new Type();
        }
        return operate(left, operation, right);
    }

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return left;
}

Type* SemanticsAnalyzer::operate(Type* left, std::string operation, Type* right) {
    std::string error = "Can not " + operation + " with " + left->toString() + " and " + right->toString() + ".";

    if (operation == "=") {
        if (!left->isVar()) throw SemanticsError("Left operand must be lvalue", token());
        if (*left < Type("string") && *right < Type("string")) return left;
        if (*left == *right) return left;
        throw SemanticsError(error, token());
    }

    std::vector<std::string> assignable = { "+=", "-=", "*=", "/=", "%=", "**=", "<<=", ">>=", "&=", "|=", "^=" };
    if (std::find(assignable.begin(), assignable.end(), operation) != assignable.end()) {
        if (!left->isVar()) throw SemanticsError("Left operand must be lvalue", token());
        operation.pop_back();
        try {
            right = operate(left, operation, right);
        }
        catch (SemanticsError err) {
            throw SemanticsError(error, token());
        }
        if (*left < Type("string") && *right < Type("string")) return left;
        try {
            return operate(left, "=", right);
        }
        catch (SemanticsError err) {
            throw SemanticsError(error, token());
        }
    }
    if (operation == "**" || operation == "*" || operation == "/" || operation == "-") {
        if (*left < Type("string") && *right < Type("string")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "+") {
        if (*left < Type("string") && *right < Type("string")) {
            if (*left < *right)
                return right;
            return left;
        }
        if (*left == Type("string") && *right == Type("char"))
            return left;
        if (*left == Type("char") && *right == Type("string"))
            return right;
        if (*left == Type("string") && *right == Type("string"))
            return left;
        throw SemanticsError(error, token());
    }

    if (operation == "<<" || operation == ">>" || operation == "%") {
        if (*left < Type("float") && *right < Type("float")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "<" || operation == ">" || operation == "<=" || operation == ">=") {
        if (*left == *right && *left < Type("range")) return new Type("bool");
        throw SemanticsError(error, token());
    }

    if (operation == "==" || operation == "!=") {
        if (*left == *right) return new Type("bool");
        throw SemanticsError(error, token());
    }

    if (operation == "&" || operation == "|" || operation == "^") {
        if (*left < Type("float") && *right < Type("float")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "&&" || operation == "||") {
        if (*left == Type("bool") && *right == Type("bool")) return left;
        throw SemanticsError(error, token());
    }

    if (operation == "in") {
        if (right->parent() != NULL && *left == *(right->parent()) ||
            *left == Type("int") && *right == Type(Type::TypeEnum::RANGE) ||
            *left == Type("char") && *right == Type("string")) return new Type("bool");
        throw SemanticsError(error, token());
    }
}

Type* SemanticsAnalyzer::operate(Type* left, std::string operation) {
    std::string error = "Can not " + operation + " with " + left->toString() + ".";
    if (operation == "!") {
        if (*left == Type("bool")) return left;
        throw SemanticsError(error, token());
    }
    if (operation == "~") {
        if (*left == Type("int")) return left;
        throw SemanticsError(error, token());
    }
    if (operation == "@" || operation == "_") {
        if (*left == Type("int") || *left == Type("float")) return left;
        if (*left == Type("bool") || *left == Type("char")) return new Type("int");
        throw SemanticsError(error, token());
    }
}

*/

// fixed

Type* SemanticsAnalyzer::EXPRESSION() {
    std::stack<std::string> st;
    int balance = 0;

    std::vector<std::pair<Type*, std::string>> rpn;

    for (;
        !(tokenName() == ":" || tokenName() == "," ||
            tokenName() == ";" || tokenName() == "}" || tokenName() == "]" ||
            (balance == 0 && tokenName() == ")"));
        tokenNext()) {
        Type* type = NULL;
        if (tokenType() == LecsicalEnum::NAME) {
            tokenNext();
            if (tokenName() == "(") {
                tokenPrev();
                type = FUNCTION_CALL();
            }
            else {
                tokenPrev();
                type = VARIABLE();
            }
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_BOOL) {
            type = CONST_BOOL();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_CHAR) {
            type = CONST_CHAR();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_INT) {
            type = CONST_INT();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_FLOAT) {
            type = CONST_FLOAT();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenType() == LecsicalEnum::CONST_RANGE) {
            type = CONST_RANGE();
            rpn.push_back({ type, "" });
        }

        if (tokenType() == LecsicalEnum::CONST_STRING) {
            type = CONST_STRING();
            rpn.push_back({ type, "" });
        }

        if (tokenName() == "(") {
            balance++;
            st.push("(");
            continue;
        }

        if (tokenName() == ")") {
            balance--;
            while (st.top() != "(") {
                rpn.push_back({ NULL, st.top() });
                st.pop();
            }
            st.pop();
            continue;
        }

        if (tokenType() == LecsicalEnum::OPERATION) {
            while (!st.empty() && st.top() != "(" && getPrior(st.top()) < getPrior(tokenName())) {
                rpn.push_back({ NULL, st.top() });
                st.pop();
            }
            while (!st.empty() && st.top() != "(" && getPrior(st.top()) == getPrior(tokenName()) && !isRightAssoc(tokenName())) {
                rpn.push_back({ NULL, st.top() });
                st.pop();
            }
            st.push(tokenName());
            continue;
        }

        if (tokenName() == ".") {
            type = METHOD_CALL((rpn.end() - 1)->first);
            tokenPrev();
            rpn.pop_back();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenName() == "[") {
            type = INDEX_CALL();
            rpn.push_back({ type, "" });
            rpn.push_back({ NULL, "[]" });
            continue;
        }

        if (isType(token())) {
            type = CAST();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenName() == "it") {
            if (analysis_stage_ == 1)
                type = tid_->varGet("it");
            else
                type = new Type();
            rpn.push_back({ type, "" });
            continue;
        }

        if (tokenName() == "{") {
            type = INITIALIZER_LIST();
            rpn.push_back({ type, "" });
            continue;
        }
    }

    while (!st.empty()) {
        if (st.top() != "(")
            rpn.push_back({ NULL, st.top() });
        st.pop();
    }

    if (analysis_stage_ == 0)
        return new Type();
    return calcRpn(rpn);

}

int SemanticsAnalyzer::getPrior(std::string op) {
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
    if (op == "in") return 13;
    if (op == "&&") return 14;
    if (op == "||") return 15;
    return 16;
}

bool SemanticsAnalyzer::isRightAssoc(std::string op) {
    for (auto& i : { "!", "~", "@", "_", "**", })
        if (op == i)
            return true;
    return false;
}

Type* SemanticsAnalyzer::FUNCTION_CALL() {
    std::string func_name = tokenName();
    tokenNext(); // from name to (
    std::vector<Type*> args;

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        Type* expr_type = EXPRESSION(); // from expression to , or )
        args.push_back(expr_type);
        if (tokenName() == ")") break;
    }

    FuncId func(func_name, args);

    if (analysis_stage_ == 1)
        function_calls_.push(func);


    if (analysis_stage_ == 1 && funcs_.find(func) == funcs_.end()) {
        std::string error = "Undeclared function: ";
        error += func_name;
        error += "(";
        for (int i = 0; i < args.size(); ++i) {
            error += args[i]->toString();
            if (i != args.size() - 1)
                error += ", ";
        }
        error += ")";

        throw SemanticsError(error, token());
    }
    tokenNext(); // from ) to smth

    tokenPrev(); // for rpn
    if (analysis_stage_ == 0) {
        return new Type();
    }
    Type* ret = funcs_[func];
    return funcs_[func];
}

Type* SemanticsAnalyzer::VARIABLE() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = tid_->varGet(tokenName());
    return ret;
}

Type* SemanticsAnalyzer::CONST_BOOL() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("bool");
    return ret;
}

Type* SemanticsAnalyzer::CONST_CHAR() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("char");
    return ret;
}

Type* SemanticsAnalyzer::CONST_INT() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("int");
    return ret;
}

Type* SemanticsAnalyzer::CONST_FLOAT() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("float");
    return ret;
}

Type* SemanticsAnalyzer::CONST_RANGE() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("range");
    return ret;
}

Type* SemanticsAnalyzer::CONST_STRING() {
    if (analysis_stage_ == 0) {
        return new Type();
    }

    Type* ret = new Type("string");
    return ret;
}

Type* SemanticsAnalyzer::INITIALIZER_LIST() {
    Type* type = new Type("array", new Type());
    Type* prev = NULL;
    while (tokenName() != "}") {
        tokenNext(); // from { to smth
        if (tokenName() == "}") break;
        Type* expr = EXPRESSION();

        if (type->parent()->baseType() == Type::TypeEnum::VOID)
            type = new Type("array", expr);
        else if (*type->parent() != *expr)
            throw SemanticsError(type->toString() + " can not contain " + expr->toString() + " elements.", token());

    }

    if (analysis_stage_ == 0) {
        return new Type();
    }

    return type;

}

Type* SemanticsAnalyzer::METHOD_CALL(Type* var_type) {
    tokenNext(); // from . to name
    std::string method_name = tokenName();
    tokenNext(); // from name to (

    std::vector<Type*> args;
    Type* one_arg = NULL;
    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        Type* expr_type = EXPRESSION(); // from expression to , or )
        one_arg = expr_type;
        args.push_back(expr_type);
        if (tokenName() == ")") break;
    }

    MethodId method(var_type, method_name, args);
    if (analysis_stage_ == 1)
        method_calls_.push(method);

    if (analysis_stage_ == 1 && (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
        method.methodType()->baseType() == Type::TypeEnum::STRING) &&
        (method_name == "size" || method_name == "popBack") &&
        method.argsNum() == 0) {
        tokenNext(); // from ) to smth
        if (method_name == "size")
            return new Type("int");
        return new Type();
    }

    if (analysis_stage_ == 1 && (method.methodType()->baseType() == Type::TypeEnum::ARRAY ||
        method.methodType()->baseType() == Type::TypeEnum::STRING) &&
        method_name == "pushBack" &&
        method.argsNum() == 1) {
        tokenNext(); // from ) to smth


        Type* m_type = method.methodType();
        if (m_type->baseType() == Type::TypeEnum::STRING)
            m_type = new Type("char");
        else
            m_type = m_type->parent();


        if (*m_type != *one_arg)
            throw SemanticsError("Can not pushBack " + one_arg->toString() + " to " + method.methodType()->toString() + ".", token());

        return new Type();
    }

    if (analysis_stage_ == 1 && methods_.find(method) == methods_.end()) {

        std::string error = "Undeclared method: ";
        error += var_type->toString();
        error += ".";
        error += method_name;
        error += "(";
        for (int i = 0; i < args.size(); ++i) {
            error += args[i]->toString();
            if (i != args.size() - 1)
                error += ", ";
        }
        error += ")";
        throw SemanticsError(error, token());
    }
    tokenNext(); // from ) to smth

    if (analysis_stage_ == 0) {
        return new Type();
    }
    return methods_[method];
}

Type* SemanticsAnalyzer::INDEX_CALL() {
    tokenNext(); // from [ to expr
    Type* type = EXPRESSION();
    return type;
}

Type* SemanticsAnalyzer::CAST() {
    Type* type = TYPE();
    tokenNext(); // from ( to smth
    Type* expr = EXPRESSION();

    if (analysis_stage_ == 0) {
        return new Type();
    }

    if (goodCast(type, expr)) {
        return type;
    }
    throw SemanticsError("Can not cast " + expr->toString() + " to " + type->toString() + ".", token());
}

Type* SemanticsAnalyzer::calcRpn(std::vector<std::pair<Type*, std::string>> rpn) {
    std::stack<Type*> st;
    for (auto& p : rpn) {
        if (p.first != NULL) {
            st.push(p.first);
            continue;
        }
        Type* ret = NULL;
        if (isUnary(p.second)) {
            Type* value = st.top();
            st.pop();
            ret = operate(value, p.second);
        }
        else {
            Type* second = st.top();
            st.pop();
            Type* first = st.top();
            st.pop();
            ret = operate(first, p.second, second);
        }
        st.push(ret);
    }

    return st.top();
}

bool SemanticsAnalyzer::isUnary(std::string oper) {
    if (oper == "!" || oper == "~" || oper == "@" || oper == "_")
        return true;
    return false;
}

Type* SemanticsAnalyzer::operate(Type* left, std::string operation, Type* right) {
    std::string error = "Can not " + operation + " with " + left->toString() + " and " + right->toString() + ".";

    if (operation == "=") {
        if (!left->isVar()) throw SemanticsError("Left operand must be lvalue", token());
        if (*left < Type("string") && *right < Type("string")) return left;
        if (left->baseType() == Type::TypeEnum::ARRAY && *right == Type("array", new Type())) return left;
        if (*left == *right) return left;
        throw SemanticsError(error, token());
    }

    std::vector<std::string> assignable = { "+=", "-=", "*=", "/=", "%=", "**=", "<<=", ">>=", "&=", "|=", "^=" };
    if (std::find(assignable.begin(), assignable.end(), operation) != assignable.end()) {
        if (!left->isVar()) throw SemanticsError("Left operand must be lvalue", token());
        operation.pop_back();
        try {
            right = operate(left, operation, right);
        }
        catch (SemanticsError err) {
            throw SemanticsError(error, token());
        }
        if (*left < Type("string") && *right < Type("string")) return left;
        try {
            return operate(left, "=", right);
        }
        catch (SemanticsError err) {
            throw SemanticsError(error, token());
        }
    }
    if (operation == "**" || operation == "*" || operation == "/" || operation == "-") {
        if (*left < Type("string") && *right < Type("string")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "+") {
        if (*left < Type("string") && *right < Type("string")) {
            if (*left < *right)
                return right;
            return left;
        }
        if (*left == Type("string") && *right == Type("char"))
            return left;
        if (*left == Type("char") && *right == Type("string"))
            return right;
        if (*left == Type("string") && *right == Type("string"))
            return left;
        throw SemanticsError(error, token());
    }

    if (operation == "<<" || operation == ">>" || operation == "%") {
        if (*left < Type("float") && *right < Type("float")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "<" || operation == ">" || operation == "<=" || operation == ">=") {
        if (*left == *right && *left < Type("range")) return new Type("bool");
        throw SemanticsError(error, token());
    }

    if (operation == "==" || operation == "!=") {
        if (*left == *right) return new Type("bool");
        throw SemanticsError(error, token());
    }

    if (operation == "&" || operation == "|" || operation == "^") {
        if (*left < Type("float") && *right < Type("float")) {
            if (*left < *right)
                return right;
            return left;
        }
        throw SemanticsError(error, token());
    }

    if (operation == "&&" || operation == "||") {
        if (*left == Type("bool") && *right == Type("bool")) return left;
        throw SemanticsError(error, token());
    }

    if (operation == "in") {
        if (right->parent() != NULL && *left == *(right->parent()) ||
            *left == Type("int") && *right == Type(Type::TypeEnum::RANGE) ||
            *left == Type("char") && *right == Type("string")) return new Type("bool");
        throw SemanticsError(error, token());
    }

    if (operation == "[]") {
        if (*right != Type("int")) throw SemanticsError("Only int expression in [] are possible.", token());
        if (*left == Type("string")) return new Type("char");
        if (left->baseType() == Type::TypeEnum::ARRAY) {
            left->parent()->setIsVar(true);
            return left->parent();
        }
        throw SemanticsError(error, token());
    }
}

Type* SemanticsAnalyzer::operate(Type* left, std::string operation) {
    std::string error = "Can not " + operation + " with " + left->toString() + ".";
    if (operation == "!") {
        if (*left == Type("bool")) return left;
        throw SemanticsError(error, token());
    }
    if (operation == "~") {
        if (*left == Type("int")) return left;
        throw SemanticsError(error, token());
    }
    if (operation == "@" || operation == "_") {
        if (*left == Type("int") || *left == Type("float")) return left;
        if (*left == Type("bool") || *left == Type("char")) return new Type("int");
        throw SemanticsError(error, token());
    }
}