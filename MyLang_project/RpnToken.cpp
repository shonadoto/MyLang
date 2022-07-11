#include "RpnToken.h"

RPN::Token::Token(RpnEnum type) : type_(type) {}

RPN::RpnEnum RPN::Token::type() { return type_; }

void RPN::Token::show() {
    switch (type_)
    {
    case RPN::RpnEnum::VAR:
        std::cout << "Variable";
        break;
    case RPN::RpnEnum::DECLARE_VAR:
        std::cout << "Declare variable";
        break;
    case RPN::RpnEnum::DELETE_VAR:
        std::cout << "Delete variable";
        break;
    case RPN::RpnEnum::GOTO:
        std::cout << "Goto";
        break;
    case RPN::RpnEnum::FALSE_GOTO:
        std::cout << "False goto";
        break;
    case RPN::RpnEnum::FOR_GOTO:
        std::cout << "For goto";
        break;
    case RPN::RpnEnum::RETURN_GOTO:
        std::cout << "Return goto";
        break;
    case RPN::RpnEnum::FUNC_CALL:
        std::cout << "Function call";
        break;
    case RPN::RpnEnum::GET_ARG:
        std::cout << "Get argument";
        break;
    case RPN::RpnEnum::PRINT:
        std::cout << "Print";
        break;
    case RPN::RpnEnum::INPUT:
        std::cout << "Input";
        break;
    case RPN::RpnEnum::KILL:
        std::cout << "Kill";
        break;
    case RPN::RpnEnum::DECLARE_ITERATOR:
        std::cout << "Declare iterator";
        break;
    case RPN::RpnEnum::MOVE_ITERATOR:
        std::cout << "Move iterator";
        break;
    case RPN::RpnEnum::SET_ITERATOR_VAR:
        std::cout << "Set iterator\'s variable";
        break;
    case RPN::RpnEnum::WHEN_COMPARE:
        std::cout << "When compare";
        break;
    case RPN::RpnEnum::OPERATION:
        std::cout << "Operation";
        break;
    case RPN::RpnEnum::CONST:
        std::cout << "Const";
        break;
    case RPN::RpnEnum::END:
        std::cout << "End";
        break;
    case RPN::RpnEnum::CREATE_ARRAY:
        std::cout << "Create array";
        break;
    case RPN::RpnEnum::POP_ARRAY:
        std::cout << "Pop array";
        break;
    case RPN::RpnEnum::PUSH_ARRAY:
        std::cout << "Push array";
        break;
    case RPN::RpnEnum::SIZE_ARRAY:
        std::cout << "Size array";
        break;
    case RPN::RpnEnum::SET_IT:
        std::cout << "Set it";
        break;
    case RPN::RpnEnum::CAST:
        std::cout << "Cast";
        break;
    default:
        std::cout << "WTF";
        break;
    }
    std::cout << "\n";
}


RPN::DeclareVar::DeclareVar(int name, Type* type)
    : name_(name), type_(type), Token(RpnEnum::DECLARE_VAR) {}

Type* RPN::DeclareVar::type() { return type_; }

int RPN::DeclareVar::name() { return name_; }

void RPN::DeclareVar::show() {
    std::cout << "Declare variable. Name: " << name_ <<
        ". Type: " << type_->toString() << ".";
    std::cout << std::endl;
}


RPN::DeleteVar::DeleteVar(int name) : name_(name), Token(RpnEnum::DELETE_VAR) {}

int RPN::DeleteVar::name() { return name_; }

void RPN::DeleteVar::show() {
    std::cout << "Delete var. Name: " << name_ << ".";
    std::cout << std::endl;
}


RPN::Goto::Goto(int address) : address_(address), Token(RpnEnum::GOTO) {}

int RPN::Goto::address() { return address_; }

void RPN::Goto::show() {
    std::cout << "Goto: " << address_ << ".";
    std::cout << std::endl;
}


RPN::FalseGoto::FalseGoto(int address) : address_(address), Token(RpnEnum::FALSE_GOTO) {}

int RPN::FalseGoto::address() { return address_; }

void RPN::FalseGoto::show() {
    std::cout << "False Goto: " << address_ << ".";
    std::cout << std::endl;
}


RPN::ForGoto::ForGoto(int address) : address_(address), Token(RpnEnum::FOR_GOTO) {}

int RPN::ForGoto::address() { return address_; }

void RPN::ForGoto::show() {
    std::cout << "For Goto: " << address_ << ".";
    std::cout << std::endl;
}


RPN::ReturnGoto::ReturnGoto() : Token(RpnEnum::RETURN_GOTO) {}


RPN::Var::Var(int name) : name_(name), Token(RpnEnum::VAR) {}

int RPN::Var::name() { return name_; }

void RPN::Var::show() {
    std::cout << "Variable. Name: " << name_ << ".";
    std::cout << std::endl;
}


RPN::FuncCall::FuncCall(int address, int args_num) :
    address_(address), args_num_(args_num), Token(RpnEnum::FUNC_CALL) {}

int RPN::FuncCall::address() { return address_; }

int RPN::FuncCall::argsNum() { return args_num_; }

void RPN::FuncCall::show() {
    std::cout << "Function call. Address: " << address_ <<
        ". Arguments number: " << args_num_ << ".";
    std::cout << std::endl;
}


RPN::GetArg::GetArg(int name) : name_(name), Token(RpnEnum::GET_ARG) {}

int RPN::GetArg::name() { return name_; }

void RPN::GetArg::show() {
    std::cout << "Get argument. Name: " << name_ << ".";
    std::cout << std::endl;
}


RPN::DelArgCnt::DelArgCnt() : Token(RpnEnum::DEL_ARG_CNT) {}



RPN::Print::Print() : Token(RpnEnum::PRINT) {}


RPN::Input::Input() : Token(RpnEnum::INPUT) {}


RPN::Kill::Kill() : Token(RpnEnum::KILL) {}


RPN::DeclareIterator::DeclareIterator() : Token(RpnEnum::DECLARE_ITERATOR) {}


RPN::MoveIterator::MoveIterator() : Token(RpnEnum::MOVE_ITERATOR) {}


RPN::SetIteratorVar::SetIteratorVar(int name) : name_(name), Token(RpnEnum::SET_ITERATOR_VAR) {}

int RPN::SetIteratorVar::name() { return name_; }

void RPN::SetIteratorVar::show() {
    std::cout << "Set iterator\'s variable. Name: " << name_ << ".";
    std::cout << std::endl;
}


RPN::WhenCompare::WhenCompare() : Token(RpnEnum::WHEN_COMPARE) {}


RPN::Operation::Operation(std::string operation) : Token(RpnEnum::OPERATION) {
    std::string op = operation;
    if (op == "[]") type_ = OperationEnum::INDEXATION;
    if (op == "**") type_ = OperationEnum::POWER;
    if (op == "!") type_ = OperationEnum::NOT;
    if (op == "~") type_ = OperationEnum::BIT_NOT;
    if (op == "@") type_ = OperationEnum::UN_PLUS;
    if (op == "_") type_ = OperationEnum::UN_MINUS;
    if (op == "*") type_ = OperationEnum::MULT;
    if (op == "/") type_ = OperationEnum::DIV;
    if (op == "%") type_ = OperationEnum::MOD;
    if (op == "+") type_ = OperationEnum::PLUS;
    if (op == "-") type_ = OperationEnum::MINUS;
    if (op == "<<") type_ = OperationEnum::BIT_LEFT;
    if (op == ">>") type_ = OperationEnum::BIT_RIGHT;
    if (op == "<") type_ = OperationEnum::LESS;
    if (op == ">") type_ = OperationEnum::MORE;
    if (op == "<=") type_ = OperationEnum::NOT_MORE;
    if (op == ">=") type_ = OperationEnum::NOT_LESS;
    if (op == "==") type_ = OperationEnum::EQUALS;
    if (op == "!=") type_ = OperationEnum::NOT_EQUALS;
    if (op == "&") type_ = OperationEnum::BIT_AND;
    if (op == "^") type_ = OperationEnum::BIT_XOR;
    if (op == "|") type_ = OperationEnum::BIT_OR;
    if (op == "in") type_ = OperationEnum::IN;
    if (op == "&&") type_ = OperationEnum::AND;
    if (op == "||") type_ = OperationEnum::OR;
    if (op == "=") type_ = OperationEnum::ASSIGN;
    if (op == "**=") type_ = OperationEnum::ASSIGN_POWER;
    if (op == "*=") type_ = OperationEnum::ASSIGN_MULT;
    if (op == "/=") type_ = OperationEnum::ASSIGN_DIV;
    if (op == "%=") type_ = OperationEnum::ASSIGN_MOD;
    if (op == "+=") type_ = OperationEnum::ASSIGN_PLUS;
    if (op == "-=") type_ = OperationEnum::ASSIGN_MINUS;
    if (op == "<<=") type_ = OperationEnum::ASSIGN_BIT_LEFT;
    if (op == ">>=") type_ = OperationEnum::ASSIGN_BIT_RIGHT;
    if (op == "&=") type_ = OperationEnum::ASSIGN_BIT_AND;
    if (op == "^=") type_ = OperationEnum::ASSIGN_BIT_XOR;
    if (op == "|=") type_ = OperationEnum::ASSIGN_BIT_OR;
}

RPN::OperationEnum RPN::Operation::type() { return type_; }

void RPN::Operation::show() {
    std::cout << "Operation: " << int(type_) << ".";
    std::cout << std::endl;
}


RPN::Const::Const(Data::Data* data) : data_(data), Token(RpnEnum::CONST) {}

Data::Data* RPN::Const::data() { return data_; }

void RPN::Const::show() {
    std::cout << "Const. Data: ";
    data_->show();
    std::cout << std::endl;
}




RPN::End::End() : Token(RpnEnum::END) {}



RPN::CreateArray::CreateArray() : Token(RpnEnum::CREATE_ARRAY) {}


RPN::PushArray::PushArray() : Token(RpnEnum::PUSH_ARRAY) {}


RPN::PopArray::PopArray() : Token(RpnEnum::POP_ARRAY) {}


RPN::SizeArray::SizeArray() : Token(RpnEnum::SIZE_ARRAY) {}


RPN::SetIt::SetIt() : Token(RpnEnum::SET_IT) {}

RPN::DelIt::DelIt() : Token(RpnEnum::DEL_IT) {}

RPN::Cast::Cast(Type* type) : type_(new Type(*type)), Token(RpnEnum::CAST) {}

Type* RPN::Cast::type() { return type_; }

void RPN::Cast::show() {
    std::cout << "Cast. Type: " << type_->toString() << ".";
    std::cout << std::endl;
}

RPN::Copy::Copy() : Token(RpnEnum::COPY) {}


RPN::PushInit::PushInit() : Token(RpnEnum::PUSH_INIT) {}