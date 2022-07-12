#include "Executor.h"

class Executor::ExecutorError {
private:
    std::string error_;
public:
    ExecutorError(std::string error) : error_(error) {}
    void sendError() {
        std::cout << "Runtime error: ";
        std::cout << error_;
        std::cout << std::endl;
    }
};


Executor::Executor(int main_start, int variable_number, std::vector<RPN::Token*> rpn) : cur_token_(main_start), rpn_(rpn) {
    memory_.resize(variable_number);
}

RPN::Token* Executor::token() { return rpn_[cur_token_]; }

RPN::RpnEnum Executor::tokenType() { return rpn_[cur_token_]->type(); }

void Executor::tokenNext() { cur_token_++; }

void Executor::tokenGoto(int address) { cur_token_ = address - 1; }

bool Executor::eqls(Data::Data* value1, Data::Data* value2) {

    Data::DataEnum type = value1->type();

    bool eqls = false;
    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        Data::Bool* f = static_cast<Data::Bool*>(value1);
        Data::Bool* s = static_cast<Data::Bool*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        Data::Char* f = static_cast<Data::Char*>(value1);
        Data::Char* s = static_cast<Data::Char*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::INT:
    {
        Data::Int* f = static_cast<Data::Int*>(value1);
        Data::Int* s = static_cast<Data::Int*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        Data::Float* f = static_cast<Data::Float*>(value1);
        Data::Float* s = static_cast<Data::Float*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::RANGE:
    {
        Data::Range* f = static_cast<Data::Range*>(value1);
        Data::Range* s = static_cast<Data::Range*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::STRING:
    {
        Data::String* f = static_cast<Data::String*>(value1);
        Data::String* s = static_cast<Data::String*>(value2);
        eqls = *f == *s;
    }
    break;
    case Data::DataEnum::ARRAY:
    {
        Data::Array* f = static_cast<Data::Array*>(value1);
        Data::Array* s = static_cast<Data::Array*>(value2);
        eqls = *f == *s;
    }
    break;
    default:
        break;
    }
    return eqls;
}

Data::Data* Executor::copy(Data::Data* data) {
    Data::DataEnum type = data->type();
    Data::Data* ret = NULL;
    switch (type)
    {
    case Data::DataEnum::BOOL:
        ret = new Data::Bool(data);
        break;
    case Data::DataEnum::CHAR:
        ret = new Data::Char(data);
        break;
    case Data::DataEnum::INT:
        ret = new Data::Int(data);
        break;
    case Data::DataEnum::FLOAT:
        ret = new Data::Float(data);
        break;
    case Data::DataEnum::RANGE:
        ret = new Data::Range(data);
        break;
    case Data::DataEnum::STRING:
        ret = new Data::String(data);
        break;
    case Data::DataEnum::ARRAY:
        ret = new Data::Array(data);
        break;
    default:
        break;
    }
    return ret;
}

Data::Data* Executor::cast(Type* type, Data::Data* data) {
    Data::Data* ret = NULL;
    if (type->parent() != NULL) {
        ret = new Data::Array();
        Data::Array* arr = static_cast<Data::Array*>(data);
        for (int i = 0; i < arr->size(); ++i) {
            Data::Data* sub = arr->get(i);
            static_cast<Data::Array*>(ret)->set(i, cast(type->parent(), sub));
        }
        return ret;
    }
    Type::TypeEnum base = type->baseType();
    switch (base)
    {
    case Type::TypeEnum::BOOL:
        ret = new Data::Bool(toBool(data));
        break;
    case Type::TypeEnum::CHAR:
        ret = new Data::Char(toChar(data));
        break;
    case Type::TypeEnum::INT:
        ret = new Data::Int(toInt(data));
        break;
    case Type::TypeEnum::FLOAT:
        ret = new Data::Float(toFloat(data));
        break;
    case Type::TypeEnum::STRING:
        ret = new Data::String(toString(data));
        break;
    default:
        break;
    }
    return ret;
}

bool Executor::toBool(Data::Data* data) {
    Data::DataEnum type = data->type();
    switch (type)
    {
    case Data::DataEnum::BOOL:
        return static_cast<Data::Bool*>(data)->get();
        break;
    case Data::DataEnum::CHAR:
        return static_cast<Data::Char*>(data)->get();
        break;
    case Data::DataEnum::INT:
        return static_cast<Data::Int*>(data)->get();
        break;
    case Data::DataEnum::FLOAT:
        return static_cast<Data::Float*>(data)->get();
        break;
    case Data::DataEnum::STRING:
        return (static_cast<Data::String*>(data)->get() == "true" ? true : false);
    default:
        break;
    }
}

char Executor::toChar(Data::Data* data) {
    Data::DataEnum type = data->type();
    switch (type)
    {
    case Data::DataEnum::BOOL:
        return static_cast<Data::Bool*>(data)->get();
        break;
    case Data::DataEnum::CHAR:
        return static_cast<Data::Char*>(data)->get();
        break;
    case Data::DataEnum::INT:
        return static_cast<Data::Int*>(data)->get();
        break;
    case Data::DataEnum::FLOAT:
        return static_cast<Data::Float*>(data)->get();
        break;
    case Data::DataEnum::STRING:
    {
        std::string str = static_cast<Data::String*>(data)->get();
        if (str.size() == 1)
            return str[0];
        throw ExecutorError("Bad cast. Can not cast String(" + str + ") to Char().");
    }
    break;
    default:
        break;
    }
}

int64_t Executor::toInt(Data::Data* data) {
    Data::DataEnum type = data->type();
    switch (type)
    {
    case Data::DataEnum::BOOL:
        return static_cast<Data::Bool*>(data)->get();
        break;
    case Data::DataEnum::CHAR:
        return static_cast<Data::Char*>(data)->get();
        break;
    case Data::DataEnum::INT:
        return static_cast<Data::Int*>(data)->get();
        break;
    case Data::DataEnum::FLOAT:
        return static_cast<Data::Float*>(data)->get();
        break;
    case Data::DataEnum::STRING:
    {
        std::string str = static_cast<Data::String*>(data)->get();
        for (auto c : str)
            if (!isdigit(c))
                throw ExecutorError("Bad cast. Can not cast String(" + str + ") to Int().");
        return std::stoll(str);
    }
    break;
    default:
        break;
    }
}

double Executor::toFloat(Data::Data* data) {
    Data::DataEnum type = data->type();
    switch (type)
    {
    case Data::DataEnum::BOOL:
        return static_cast<Data::Bool*>(data)->get();
        break;
    case Data::DataEnum::CHAR:
        return static_cast<Data::Char*>(data)->get();
        break;
    case Data::DataEnum::INT:
        return static_cast<Data::Int*>(data)->get();
        break;
    case Data::DataEnum::FLOAT:
        return static_cast<Data::Float*>(data)->get();
        break;
    case Data::DataEnum::STRING: {
        std::string str = static_cast<Data::String*>(data)->get();
        if (!isFloat(str))
            throw ExecutorError("Bad cast. Can not cast String(" + str + ") to Float().");
        return std::stod(str);
    }
        break;
    default:
        break;
    }
}

std::string Executor::toString(Data::Data* data) {

    Data::DataEnum type = data->type();
    std::string ret = "";
    switch (type)
    {
    case Data::DataEnum::BOOL:
        ret =  static_cast<Data::Bool*>(data)->get() ? "true" : "false";
        break;
    case Data::DataEnum::CHAR:
        ret =  (static_cast<Data::Char*>(data)->get());
        break;
    case Data::DataEnum::INT:
        ret = std::to_string(static_cast<Data::Int*>(data)->get());
        break;
    case Data::DataEnum::FLOAT:
        ret = std::to_string(static_cast<Data::Float*>(data)->get());
        break;
    case Data::DataEnum::STRING:
    {
        ret= static_cast<Data::String*>(data)->get();
    }
    break;
    default:
        break;
    }
    return ret;
}

bool Executor::isFloat(std::string str) {
    int i = 0;
    if (str.size() == 0) return false;
    if (!isdigit(str[i])) return false;
    while (i < str.size() && isdigit(str[i])) {
        i++;
    }
    if (i >= str.size())
        return true;
    if (str[i] != '.')
        return false;
    i++;
    if (i >= str.size()) return true;
    while (i < str.size() && isdigit(str[i])) {
        i++;
    }
    if (i >= str.size())
        return true;
    if (str[i] != 'E' && str[i] != 'e')
        return false;
    i++;
    if (i >= str.size())
        return false;
    if (str[i] == '+' || str[i] == '-') {
        i++;
        if (i >= str.size())
            return false;
    }
    if (!isdigit(str[i]))
        return false;
    while (i < str.size() && isdigit(str[i])) {
        i++;
    }
    if (i >= str.size()) return true;
    return false;
    
}








void Executor::work() {

    try {
        for (int i = 0; i < rpn_.size(); ++i) {
            if (rpn_[i]->type() == RPN::RpnEnum::END) {
                return_address_.push(i);
                break;
            }
        }

        args_num_.push(0);
        cur_token_ = 0;

        for (; tokenType() != RPN::RpnEnum::END; tokenNext()) {
            switch (tokenType())
            {
            case RPN::RpnEnum::VAR:
                VAR();
                break;
            case RPN::RpnEnum::DECLARE_VAR:
                DECLARE_VAR();
                break;
            case RPN::RpnEnum::DELETE_VAR:
                DELETE_VAR();
                break;
            case RPN::RpnEnum::GOTO:
                GOTO();
                break;
            case RPN::RpnEnum::FALSE_GOTO:
                FALSE_GOTO();
                break;
            case RPN::RpnEnum::FOR_GOTO:
                FOR_GOTO();
                break;
            case RPN::RpnEnum::RETURN_GOTO:
                RETURN_GOTO();
                break;
            case RPN::RpnEnum::FUNC_CALL:
                FUNC_CALL();
                break;
            case RPN::RpnEnum::GET_ARG:
                GET_ARG();
                break;
            case RPN::RpnEnum::DEL_ARG_CNT:
                DEL_ARG_CNT();
                break;
            case RPN::RpnEnum::PRINT:
                PRINT();
                break;
            case RPN::RpnEnum::INPUT:
                INPUT();
                break;
            case RPN::RpnEnum::KILL:
                KILL();
                break;
            case RPN::RpnEnum::DECLARE_ITERATOR:
                DECLARE_ITERATOR();
                break;
            case RPN::RpnEnum::MOVE_ITERATOR:
                MOVE_ITERATOR();
                break;
            case RPN::RpnEnum::SET_ITERATOR_VAR:
                SET_ITERATOR_VAR();
                break;
            case RPN::RpnEnum::DELETE_ITERATOR:
                DELETE_ITERATOR();
                break;
            case RPN::RpnEnum::WHEN_COMPARE:
                WHEN_COMPARE();
                break;
            case RPN::RpnEnum::OPERATION:
                OPERATION();
                break;
            case RPN::RpnEnum::CONST:
                CONST();
                break;
            case RPN::RpnEnum::END:
                END();
                break;
            case RPN::RpnEnum::CREATE_ARRAY:
                CREATE_ARRAY();
                break;
            case RPN::RpnEnum::PUSH_ARRAY:
                PUSH_ARRAY();
                break;
            case RPN::RpnEnum::POP_ARRAY:
                POP_ARRAY();
                break;
            case RPN::RpnEnum::SIZE_ARRAY:
                SIZE_ARRAY();
                break;
            case RPN::RpnEnum::SET_IT:
                SET_IT();
                break;
            case RPN::RpnEnum::DEL_IT:
                DEL_IT();
                break;
            case RPN::RpnEnum::CAST:
                CAST();
                break;
            case RPN::RpnEnum::COPY:
                COPY();
                break;
            case RPN::RpnEnum::PUSH_INIT:
                PUSH_INIT();
                break;
            default:
                break;
            }
        }
    }
    catch (ExecutorError error) {
        error.sendError();
    }
}


void Executor::VAR() {
    RPN::Var* var = static_cast<RPN::Var*>(token());
    main_stack_.push(memory_[var->name()].top());
}

void Executor::DECLARE_VAR() {
    RPN::DeclareVar* decl_var = static_cast<RPN::DeclareVar*>(token());
    int name = decl_var->name();
    Type::TypeEnum base_type = decl_var->type()->baseType();
    Data::Data* data;
    switch (base_type)
    {
    case Type::TypeEnum::BOOL:
        data = new Data::Bool();
        break;
    case Type::TypeEnum::CHAR:
        data = new Data::Char();
        break;
    case Type::TypeEnum::INT:
        data = new Data::Int();
        break;
    case Type::TypeEnum::FLOAT:
        data = new Data::Float();
        break;
    case Type::TypeEnum::STRING:
        data = new Data::String();
        break;
    case Type::TypeEnum::RANGE_2:
        data = new Data::Range();
        break;
    case Type::TypeEnum::RANGE_3:
        data = new Data::Range();
        break;
    case Type::TypeEnum::ARRAY:
        data = new Data::Array();
        break;
    case Type::TypeEnum::VOID:
        throw ExecutorError("Void variable type");
        break;
    default:
        break;
    }

    memory_[name].push(data);
}

void Executor::DELETE_VAR() {
    RPN::DeleteVar* del_var = static_cast<RPN::DeleteVar*>(token());
    int name = del_var->name();
    delete memory_[name].top();
    memory_[name].pop();
}

void Executor::GOTO() {
    RPN::Goto* go_to = static_cast<RPN::Goto*>(token());
    int address = go_to->address();
    tokenGoto(address);
}

void Executor::FALSE_GOTO() {
    RPN::FalseGoto* false_goto = static_cast<RPN::FalseGoto*>(token());
    int address = false_goto->address();
    Data::Bool* value = static_cast<Data::Bool*>(main_stack_.top());
    main_stack_.pop();
    if (value->get() == false)
        tokenGoto(address);
}

void Executor::RETURN_GOTO() {
    int address = return_address_.top();
    return_address_.pop();
    tokenGoto(address);
}

void Executor::FOR_GOTO() {
    RPN::ForGoto* for_goto = static_cast<RPN::ForGoto*>(token());
    int address = for_goto->address();

    int it = iterators_.top();

    Data::Data* value = main_stack_.top();

    if (!value->in_range(it)) {
        main_stack_.pop();
        tokenGoto(address);
    }

}

void Executor::FUNC_CALL() {
    RPN::FuncCall* call = static_cast<RPN::FuncCall*>(token());
    int address = call->address();
    int args_num = call->argsNum();
    return_address_.push(cur_token_ + 1);

    args_num_.push(args_num);

    tokenGoto(address);
}

void Executor::GET_ARG() {
    RPN::GetArg* get_arg = static_cast<RPN::GetArg*>(token());

    int name = get_arg->name();


    int args_num = args_num_.top()--;
    if (args_num <= 0) {
        return;
    }
    std::stack<Data::Data*> tmp;
    while (args_num--) {
        tmp.push(main_stack_.top());
        main_stack_.pop();
    }

    Data::Data* arg = memory_[name].top();
    Data::Data* value = tmp.top();


    delete arg;
    memory_[name].top() = copy(value);

    tmp.pop();
    while (!tmp.empty())
    {
        main_stack_.push(tmp.top());
        tmp.pop();
    }
}

void Executor::DEL_ARG_CNT() {
    args_num_.pop();
}

void Executor::PRINT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    value->print();
}

void Executor::INPUT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    value->input();
}

void Executor::KILL() {
    main_stack_.pop();
}

void Executor::DECLARE_ITERATOR() {
    iterators_.push(0);
}

void Executor::MOVE_ITERATOR() {
    iterators_.top()++;
}

void Executor::SET_ITERATOR_VAR() {
    RPN::SetIteratorVar* set_it_var = static_cast<RPN::SetIteratorVar*>(token());
    int name = set_it_var->name();
    Data::Data* data = main_stack_.top();
    Data::DataEnum type = data->type();
    if (type == Data::DataEnum::RANGE) {
        Data::Range* range = static_cast<Data::Range*>(data);
        int64_t ind = iterators_.top();
        if (!range->in_range(ind))
            throw ExecutorError("Index is out of range.");
        int64_t value = range->get(ind);
        static_cast<Data::Int*>(memory_[name].top())->set(value);
        return;
    }
    if (type == Data::DataEnum::STRING) {
        Data::String* str = static_cast<Data::String*>(data);
        int64_t ind = iterators_.top();
        if (!str->in_range(ind))
            throw ExecutorError("Index out of range.");
        char value = str->get(ind);
        static_cast<Data::Char*>(memory_[name].top())->set(value);
        return;
    }
    if (type == Data::DataEnum::ARRAY) {
        Data::Array* arr = static_cast<Data::Array*>(data);
        int64_t ind = iterators_.top();
        if (!arr->in_range(ind))
            throw ExecutorError("Index out of range.");
        Data::Data* value = arr->get(ind);
        Data::DataEnum value_type = value->type();
        Data::Data* push = copy(value);
        delete memory_[name].top();
        memory_[name].top() = push;
        return;
    }

    throw "Smth 2 in set iterator\'s variable.";
}

void Executor::DELETE_ITERATOR() {
    iterators_.pop();
}

void Executor::WHEN_COMPARE() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    bool eq = eqls(value1, value2);

    main_stack_.push(value1);
    main_stack_.push(new Data::Bool(eq));
}

void Executor::CONST() {
    RPN::Const* cons = static_cast<RPN::Const*>(token());
    main_stack_.push(copy(cons->data()));
}

void Executor::END() {
    throw ExecutorError("U R not supposed to be here.");
}

void Executor::CREATE_ARRAY() {
    main_stack_.push(new Data::Array());
}

void Executor::PUSH_ARRAY() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();

    Data::Data* dt = memory_[0].top();

    std::swap(value, dt);

    if (dt->type() == Data::DataEnum::STRING) {
        Data::String* str = static_cast<Data::String*>(dt);
        str->set(str->size(), static_cast<Data::Char*>(value)->get());
    }
    else if (dt->type() == Data::DataEnum::ARRAY) {
        Data::Array* arr = static_cast<Data::Array*>(dt);
        arr->set(arr->size(), copy(value));
    }
    else
        throw "Push array wtf";
    main_stack_.push(new Data::Bool());
}

void Executor::POP_ARRAY() {
    Data::Data* value = memory_[0].top();
    value->pop();
    main_stack_.push(new Data::Bool());
}

void Executor::SIZE_ARRAY() {
    Data::Data* value = memory_[0].top();
    main_stack_.push(new Data::Int(value->size()));
}

void Executor::SET_IT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    memory_[0].push(value);
}

void Executor::DEL_IT() {
    memory_[0].pop();
}

void Executor::CAST() {
    RPN::Cast* cst = static_cast<RPN::Cast*>(token());
    Type* type = cst->type();
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    Data::Data* push = cast(type, value);
    main_stack_.push(push);

}

void Executor::COPY() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    main_stack_.push(copy(value));
}

void Executor::PUSH_INIT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();

    Data::Data* dt = main_stack_.top();
    main_stack_.pop();

    if (dt->type() == Data::DataEnum::STRING) {
        Data::String* str = static_cast<Data::String*>(dt);
        str->set(str->size(), static_cast<Data::Char*>(value)->get());
    }
    if (dt->type() == Data::DataEnum::ARRAY) {
        Data::Array* arr = static_cast<Data::Array*>(dt);
        arr->set(arr->size(), value);
    }
    else
        throw "Push array wtf";

    main_stack_.push(dt);
}





void Executor::OPERATION() {
    RPN::Operation* oper = static_cast<RPN::Operation*>(token());
    RPN::OperationEnum oper_type = oper->type();
    switch (oper_type)
    {
    case RPN::OperationEnum::INDEXATION:
        INDEXATION();
        break;
    case RPN::OperationEnum::POWER:
        POWER();
        break;
    case RPN::OperationEnum::NOT:
        NOT();
        break;
    case RPN::OperationEnum::BIT_NOT:
        BIT_NOT();
        break;
    case RPN::OperationEnum::UN_PLUS:
        UN_PLUS();
        break;
    case RPN::OperationEnum::UN_MINUS:
        UN_MINUS();
        break;
    case RPN::OperationEnum::MULT:
        MULT();
        break;
    case RPN::OperationEnum::DIV:
        DIV();
        break;
    case RPN::OperationEnum::MOD:
        MOD();
        break;
    case RPN::OperationEnum::PLUS:
        PLUS();
        break;
    case RPN::OperationEnum::MINUS:
        MINUS();
        break;
    case RPN::OperationEnum::BIT_LEFT:
        BIT_LEFT();
        break;
    case RPN::OperationEnum::BIT_RIGHT:
        BIT_RIGHT();
        break;
    case RPN::OperationEnum::LESS:
        LESS();
        break;
    case RPN::OperationEnum::MORE:
        MORE();
        break;
    case RPN::OperationEnum::NOT_LESS:
        NOT_LESS();
        break;
    case RPN::OperationEnum::NOT_MORE:
        NOT_MORE();
        break;
    case RPN::OperationEnum::EQUALS:
        EQUALS();
        break;
    case RPN::OperationEnum::NOT_EQUALS:
        NOT_EQUALS();
        break;
    case RPN::OperationEnum::BIT_AND:
        BIT_AND();
        break;
    case RPN::OperationEnum::BIT_XOR:
        BIT_XOR();
        break;
    case RPN::OperationEnum::BIT_OR:
        BIT_OR();
        break;
    case RPN::OperationEnum::IN:
        IN();
        break;
    case RPN::OperationEnum::AND:
        AND();
        break;
    case RPN::OperationEnum::OR:
        OR();
        break;
    case RPN::OperationEnum::ASSIGN:
        ASSIGN();
        break;
    case RPN::OperationEnum::ASSIGN_PLUS:
        ASSIGN_PLUS();
        break;
    case RPN::OperationEnum::ASSIGN_MINUS:
        ASSIGN_MINUS();
        break;
    case RPN::OperationEnum::ASSIGN_MULT:
        ASSIGN_MULT();
        break;
    case RPN::OperationEnum::ASSIGN_DIV:
        ASSIGN_DIV();
        break;
    case RPN::OperationEnum::ASSIGN_MOD:
        ASSIGN_MOD();
        break;
    case RPN::OperationEnum::ASSIGN_POWER:
        ASSIGN_POWER();
        break;
    case RPN::OperationEnum::ASSIGN_BIT_LEFT:
        ASSIGN_BIT_LEFT();
        break;
    case RPN::OperationEnum::ASSIGN_BIT_RIGHT:
        ASSIGN_BIT_RIGHT();
        break;
    case RPN::OperationEnum::ASSIGN_BIT_AND:
        ASSIGN_BIT_AND();
        break;
    case RPN::OperationEnum::ASSIGN_BIT_OR:
        ASSIGN_BIT_OR();
        break;
    case RPN::OperationEnum::ASSIGN_BIT_XOR:
        ASSIGN_BIT_XOR();
        break;
    case RPN::OperationEnum::DOUBLE_POINT:
        DOUBLE_POINT();
        break;
    default:
        break;
    }
}

void Executor::INDEXATION() {
    Data::Data* ind = main_stack_.top();
    main_stack_.pop();
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    int64_t i = static_cast<Data::Int*>(ind)->get();
    if (value->type() == Data::DataEnum::STRING) {
        Data::String* str = static_cast<Data::String*>(value);
        if (!str->in_range(i))
            throw ExecutorError("Index out of range");
        char c = str->get(i);
        main_stack_.push(new Data::Char(c));
        return;
    }
    if (value->type() == Data::DataEnum::ARRAY) {
        Data::Array* arr = static_cast<Data::Array*>(value);
        if (!arr->in_range(i))
            throw ExecutorError("Index out of range");
        Data::Data* x = arr->get(i);
        main_stack_.push(x);
        return;
    }
    throw ExecutorError("Indexation shit");
}


void Executor::POWER() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = std::pow(val1, val2);
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = std::pow(val1, val2);
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = std::pow(val1, val2);
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = std::pow(val1, val2);
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = std::pow(val1, val2);
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = std::pow(val1, val2);
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = std::pow(val1, val2);
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = std::pow(val1, val2);
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = std::pow(val1, val2);
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = std::pow(val1, val2);
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    break;
    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::MULT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 * val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 * val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 * val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 * val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 * val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 * val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 * val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 * val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 * val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 * val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    break;
    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::DIV() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->is_zero())
        throw ExecutorError("Division by zero.");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 / val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 / val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 / val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 / val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 / val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 / val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 / val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 / val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 / val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 / val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    break;
    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::MINUS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 - val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 - val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 - val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 - val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 - val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 - val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 - val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 - val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 - val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 - val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    break;
    default:
        break;
    }

    main_stack_.push(push);
}


void Executor::PLUS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 + val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 + val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 + val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 + val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 + val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 + val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::STRING:
        {
            std::string val2 = static_cast<Data::String*>(value2)->get();
            std::string res = val1 + val2;
            push = new Data::String(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 + val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 + val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 + val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            double res = val1 + val2;
            push = new Data::Float(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            std::string res = val1 + val2;
            push = new Data::String(res);
        }
        break;
        case Data::DataEnum::STRING:
        {
            std::string val2 = static_cast<Data::String*>(value2)->get();
            std::string res = val1 + val2;
            push = new Data::String(res);
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }

    main_stack_.push(push);
}


void Executor::BIT_LEFT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->less_zero())
        throw ExecutorError("Shift by negative number");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 << val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 << val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 << val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 << val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 << val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 << val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 << val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 << val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 << val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::BIT_RIGHT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->less_zero())
        throw ExecutorError("Shift by negative number");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 >> val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 >> val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 >> val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 >> val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 >> val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 >> val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 >> val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 >> val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 >> val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::MOD() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->is_zero())
        throw ExecutorError("Division by zero.");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 % val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 % val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 % val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 % val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 % val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 % val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 % val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 % val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 % val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}


void Executor::LESS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 < val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 < val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 < val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 < val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 < val2;
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}

void Executor::MORE() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 > val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 > val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 > val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 > val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 > val2;
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}

void Executor::NOT_LESS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 >= val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 >= val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 >= val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 >= val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 >= val2;
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}

void Executor::NOT_MORE() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 <= val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 <= val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 <= val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 <= val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 <= val2;
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}


void Executor::EQUALS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 == val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 == val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 == val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 == val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 == val2;
    }
    break;
    case Data::DataEnum::RANGE:
    {
        Data::Range* val1 = static_cast<Data::Range*>(value1);
        Data::Range* val2 = static_cast<Data::Range*>(value2);
        res = *val1 == *val2;
    }
    break;
    case Data::DataEnum::ARRAY:
    {
        Data::Array* val1 = static_cast<Data::Array*>(value1);
        Data::Array* val2 = static_cast<Data::Array*>(value2);
        res = *val1 == *val2;
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}

void Executor::NOT_EQUALS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();
    Data::Data* push;

    bool res;


    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        bool val2 = static_cast<Data::Bool*>(value2)->get();
        res = val1 != val2;
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        char val2 = static_cast<Data::Char*>(value2)->get();
        res = val1 != val2;
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        int64_t val2 = static_cast<Data::Int*>(value2)->get();
        res = val1 != val2;
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        double val2 = static_cast<Data::Float*>(value2)->get();
        res = val1 != val2;
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        std::string val2 = static_cast<Data::String*>(value2)->get();
        res = val1 != val2;
    }
    break;
    case Data::DataEnum::RANGE:
    {
        Data::Range* val1 = static_cast<Data::Range*>(value1);
        Data::Range* val2 = static_cast<Data::Range*>(value2);
        res = !(*val1 == *val2);
    }
    break;
    case Data::DataEnum::ARRAY:
    {
        Data::Array* val1 = static_cast<Data::Array*>(value1);
        Data::Array* val2 = static_cast<Data::Array*>(value2);
        res = !(*val1 == *val2);
    }
    break;
    default:
        break;
    }
    push = new Data::Bool(res);
    main_stack_.push(push);
}


void Executor::BIT_AND() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 & val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 & val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 & val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 & val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 & val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 & val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 & val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 & val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 & val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::BIT_OR() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 | val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 | val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 | val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 | val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 | val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 | val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 | val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 | val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 | val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::BIT_XOR() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            bool res = val1 ^ val2;
            push = new Data::Bool(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 ^ val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 ^ val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            char res = val1 ^ val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            char res = val1 ^ val2;
            push = new Data::Char(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 ^ val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            int64_t res = val1 ^ val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            int64_t res = val1 ^ val2;
            push = new Data::Int(res);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            int64_t res = val1 ^ val2;
            push = new Data::Int(res);
        }
        break;

        default:
            break;
        }
    }
    break;

    default:
        break;
    }

    main_stack_.push(push);
}


void Executor::AND() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();

    Data::Data* push;

    bool val1 = static_cast<Data::Bool*>(value1)->get();
    bool val2 = static_cast<Data::Bool*>(value2)->get();


    bool res = val1 && val2;

    push = new Data::Bool(res);

    main_stack_.push(push);
}

void Executor::OR() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type = value1->type();

    Data::Data* push;

    bool val1 = static_cast<Data::Bool*>(value1)->get();
    bool val2 = static_cast<Data::Bool*>(value2)->get();


    bool res = val1 || val2;

    push = new Data::Bool(res);

    main_stack_.push(push);
}


void Executor::IN() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    bool res = false;

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        for (auto& i : static_cast<Data::Array*>(value2)->get()) {
            Data::Bool* x = static_cast<Data::Bool*>(i);
            if (x->get() == val1)
                res = true;
        }
    }
    break;
    case Data::DataEnum::CHAR:
    {
        

        char val1 = static_cast<Data::Char*>(value1)->get();
        
        switch (type2)
        {

        case Data::DataEnum::STRING:
        {
            std::string str = static_cast<Data::String*>(value2)->get();
            res = std::find(str.begin(), str.end(), val1) != str.end();
        }
            break;
        case Data::DataEnum::ARRAY:
        {
            for (auto& i : static_cast<Data::Array*>(value2)->get()) {
                Data::Char* x = static_cast<Data::Char*>(i);
                if (x->get() == val1)
                    res = true;
            }
        }
            break;
        default:
            break;
        }

    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();

        switch (type2)
        {
        case Data::DataEnum::RANGE:
        {
            Data::Range* val2 = static_cast<Data::Range*>(value2);
            int64_t from = val2->from(), to = val2->to(), step = val2->step();
            res = from <= val1 && val1 <= to && (val1 - from) % step == 0;
        }
        break;
        case Data::DataEnum::ARRAY:
        {

            for (auto& i : static_cast<Data::Array*>(value2)->get()) {
                Data::Int* x = static_cast<Data::Int*>(i);
                if (x->get() == val1)
                    res = true;
            }
        }
        break;
        default:
            break;
        }

    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        for (auto& i : static_cast<Data::Array*>(value2)->get()) {
            Data::Float* x = static_cast<Data::Float*>(i);
            if (x->get() == val1)
                res = true;
        }
    }
    break;
    case Data::DataEnum::RANGE:
    {
        Data::Range* val1 = static_cast<Data::Range*>(value1);
        for (auto& i : static_cast<Data::Array*>(value2)->get()) {
            Data::Range* x = static_cast<Data::Range*>(i);
            if (*x == *val1)
                res = true;
        }
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        for (auto& i : static_cast<Data::Array*>(value2)->get()) {
            Data::String* x = static_cast<Data::String*>(i);
            if (x->get() == val1)
                res = true;
        }
    }
    break;
    case Data::DataEnum::ARRAY:
    {
        Data::Array* val1 = static_cast<Data::Array*>(value1);
        for (auto& i : static_cast<Data::Array*>(value2)->get()) {
            Data::Array* x = static_cast<Data::Array*>(i);
            if (*x == *val1)
                res = true;
        }
    }
    break;
    default:
        break;
    }

    push = new Data::Bool(res);
    main_stack_.push(push);
}


void Executor::NOT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    bool res = !static_cast<Data::Bool*>(value)->get();
    Data::Data* push = new Data::Bool(res);
    main_stack_.push(push);
}

void Executor::BIT_NOT() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    int64_t res = ~static_cast<Data::Int*>(value)->get();
    Data::Data* push = new Data::Int(res);
    main_stack_.push(push);
}

void Executor::UN_PLUS() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    Data::DataEnum type = value->type();
    Data::Data* push;
    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val = static_cast<Data::Bool*>(value)->get();
        int64_t res = +int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val = static_cast<Data::Char*>(value)->get();
        int64_t res = +int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val = static_cast<Data::Int*>(value)->get();
        int64_t res = +int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val = static_cast<Data::Float*>(value)->get();
        double res = +double(val);
        push = new Data::Float(res);
    }
    break;
    default:
        break;
    }

    main_stack_.push(push);
}

void Executor::UN_MINUS() {
    Data::Data* value = main_stack_.top();
    main_stack_.pop();
    Data::DataEnum type = value->type();
    Data::Data* push;
    switch (type)
    {
    case Data::DataEnum::BOOL:
    {
        bool val = static_cast<Data::Bool*>(value)->get();
        int64_t res = -int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val = static_cast<Data::Char*>(value)->get();
        int64_t res = -int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val = static_cast<Data::Int*>(value)->get();
        int64_t res = -int64_t(val);
        push = new Data::Int(res);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val = static_cast<Data::Float*>(value)->get();
        double res = -double(val);
        push = new Data::Float(res);
    }
    break;
    default:
        break;
    }

    main_stack_.push(push);
}




void Executor::ASSIGN() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::RANGE:
    {
        int64_t from = static_cast<Data::Range*>(value2)->from();
        int64_t to = static_cast<Data::Range*>(value2)->to();
        int64_t step = static_cast<Data::Range*>(value2)->step();

        static_cast<Data::Range*>(value1)->set(from, to, step);
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val = static_cast<Data::String*>(value2)->get();
        static_cast<Data::String*>(value1)->set(val);
    }
    break;
    case Data::DataEnum::ARRAY:
    {
        std::vector<Data::Data*> val = static_cast<Data::Array*>(value2)->get();
        static_cast<Data::Array*>(value1)->set(val);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_PLUS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 += val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 += val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 += val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 += val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::STRING:
    {
        std::string val1 = static_cast<Data::String*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 += val2;
        }
        break;
        case Data::DataEnum::STRING:
        {
            std::string val2 = static_cast<Data::String*>(value2)->get();
            val1 += val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::String*>(value1)->set(val1);
    }
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_MINUS() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 -= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 -= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 -= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 -= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 -= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_MULT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 *= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 *= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 *= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 *= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 *= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_DIV() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->is_zero())
        throw ExecutorError("Division by zero.");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 /= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 /= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 /= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 /= val2;
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 /= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_POWER() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::FLOAT:
    {
        double val1 = static_cast<Data::Float*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        case Data::DataEnum::FLOAT:
        {
            double val2 = static_cast<Data::Float*>(value2)->get();
            val1 = std::pow(val1, val2);
        }
        break;
        default:
            break;
        }
        static_cast<Data::Float*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_MOD() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->is_zero())
        throw ExecutorError("Division by zero.");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 %= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 %= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 %= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 %= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_BIT_LEFT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->less_zero())
        throw ExecutorError("Shift by negative number");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 <<= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 <<= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 <<= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 <<= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_BIT_RIGHT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    if (value2->less_zero())
        throw ExecutorError("Shift by negative number");

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 >>= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 >>= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 >>= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 >>= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_BIT_AND() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 &= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 &= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 &= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 &= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_BIT_OR() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 |= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 |= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 |= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 |= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}

void Executor::ASSIGN_BIT_XOR() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::BOOL:
    {
        bool val1 = static_cast<Data::Bool*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 ^= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Bool*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::CHAR:
    {
        char val1 = static_cast<Data::Char*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 ^= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Char*>(value1)->set(val1);
    }
    break;
    case Data::DataEnum::INT:
    {
        int64_t val1 = static_cast<Data::Int*>(value1)->get();
        switch (type2)
        {
        case Data::DataEnum::BOOL:
        {
            bool val2 = static_cast<Data::Bool*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::CHAR:
        {
            char val2 = static_cast<Data::Char*>(value2)->get();
            val1 ^= val2;
        }
        break;
        case Data::DataEnum::INT:
        {
            int64_t val2 = static_cast<Data::Int*>(value2)->get();
            val1 ^= val2;
        }
        break;
        default:
            break;
        }
        static_cast<Data::Int*>(value1)->set(val1);
    }
    break;
    default:
        break;
    }
    main_stack_.push(value1);
}


void Executor::DOUBLE_POINT() {
    Data::Data* value2 = main_stack_.top();
    main_stack_.pop();
    Data::Data* value1 = main_stack_.top();
    main_stack_.pop();

    Data::DataEnum type1 = value1->type(), type2 = value2->type();

    Data::Data* push;

    switch (type1)
    {
    case Data::DataEnum::INT: {
        int val1 = static_cast<Data::Int*>(value1)->get();
        int val2 = static_cast<Data::Int*>(value2)->get();
        push = new Data::Range(val1, val2, 1);
    }
        break;
    case Data::DataEnum::RANGE:
    {
        int from = static_cast<Data::Range*>(value1)->from();
        int to = static_cast<Data::Range*>(value1)->to();
        int step = static_cast<Data::Int*>(value2)->get();
        push = new Data::Range(from, to, step);
    }
        break;
    default:
        break;
    }

    main_stack_.push(push);
}