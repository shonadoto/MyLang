#ifndef EXECUTOR
#define EXECUTOR

#include "RpnCreator.h"

#pragma once
class Executor
{
private:

    class ExecutorError;

    std::vector<RPN::Token*> rpn_;
    int cur_token_;
    std::stack<Data::Data*> main_stack_;
    std::stack<int> return_address_;
    std::stack<int> args_num_;
    std::stack<int> iterators_;
    std::vector<std::stack<Data::Data*>> memory_;

public:
    Executor(int main_start, int variable_number, std::vector<RPN::Token*> rpn);
    void work();
private:
    RPN::Token* token();
    RPN::RpnEnum tokenType();
    void tokenNext();
    void tokenGoto(int address);

    bool eqls(Data::Data* value1, Data::Data* value2);
    Data::Data* copy(Data::Data* data);
    Data::Data* cast(Type* type, Data::Data* data);
    bool toBool(Data::Data* data);
    char toChar(Data::Data* data);
    int64_t toInt(Data::Data* data);
    double toFloat(Data::Data* data);
    std::string toString(Data::Data* data);
    bool isFloat(std::string str);

    void VAR();
    void DECLARE_VAR();
    void DELETE_VAR();
    void GOTO();
    void FALSE_GOTO();
    void FOR_GOTO();
    void RETURN_GOTO();
    void FUNC_CALL();
    void GET_ARG();
    void DEL_ARG_CNT();
    void PRINT();
    void INPUT();
    void KILL();
    void DECLARE_ITERATOR();
    void MOVE_ITERATOR();
    void SET_ITERATOR_VAR();
    void WHEN_COMPARE();
    void OPERATION();
    void CONST();
    void END();
    void CREATE_ARRAY();
    void PUSH_ARRAY();
    void POP_ARRAY();
    void SIZE_ARRAY();
    void SET_IT();
    void DEL_IT();
    void CAST();
    void COPY();
    void PUSH_INIT();


    void INDEXATION();
    void POWER();
    void NOT();
    void BIT_NOT();
    void UN_PLUS();
    void UN_MINUS();
    void MULT();
    void DIV();
    void MOD();
    void PLUS();
    void MINUS();
    void BIT_LEFT();
    void BIT_RIGHT();
    void LESS();
    void MORE();
    void NOT_LESS();
    void NOT_MORE();
    void EQUALS();
    void NOT_EQUALS();
    void BIT_AND();
    void BIT_XOR();
    void BIT_OR();
    void IN();
    void AND();
    void OR();
    void ASSIGN();
    void ASSIGN_PLUS();
    void ASSIGN_MINUS();
    void ASSIGN_MULT();
    void ASSIGN_DIV();
    void ASSIGN_MOD();
    void ASSIGN_POWER();
    void ASSIGN_BIT_LEFT();
    void ASSIGN_BIT_RIGHT();
    void ASSIGN_BIT_AND();
    void ASSIGN_BIT_OR();
    void ASSIGN_BIT_XOR();
};


#endif // !EXECUTOR
