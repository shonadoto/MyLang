#ifndef RPN_TOKEN
#define RPN_TOKEN

#pragma once
#include "Data.h"

namespace RPN {

    enum class OperationEnum
    {
        INDEXATION,
        POWER,
        NOT, BIT_NOT, UN_PLUS, UN_MINUS,
        MULT, DIV, MOD,
        PLUS, MINUS,
        BIT_LEFT, BIT_RIGHT,
        LESS, MORE, NOT_LESS, NOT_MORE,
        EQUALS, NOT_EQUALS,
        BIT_AND,
        BIT_XOR,
        BIT_OR,
        IN,
        AND,
        OR,
        ASSIGN,
        ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_MULT, ASSIGN_DIV, ASSIGN_MOD,
        ASSIGN_POWER, ASSIGN_BIT_LEFT, ASSIGN_BIT_RIGHT,
        ASSIGN_BIT_AND, ASSIGN_BIT_OR, ASSIGN_BIT_XOR,

        DOUBLE_POINT
    };

    enum class RpnEnum {
        VAR, DECLARE_VAR, DELETE_VAR, GOTO, FALSE_GOTO, FOR_GOTO, RETURN_GOTO,
        FUNC_CALL, GET_ARG, DEL_ARG_CNT, PRINT, INPUT, KILL, 
        DECLARE_ITERATOR, MOVE_ITERATOR, SET_ITERATOR_VAR, DELETE_ITERATOR,
        WHEN_COMPARE, OPERATION, CONST, END, 
        CREATE_ARRAY, PUSH_ARRAY, POP_ARRAY, SIZE_ARRAY, SET_IT, DEL_IT,
        CAST,
        COPY,
        PUSH_INIT
    };


    class Token
    {
    private:
        RpnEnum type_;
    public:
        Token(RpnEnum type);
        Token(Token* other);
        RpnEnum type();
        virtual void show();
    };

    class DeclareVar : public Token {
    private:
        int name_;
        Type* type_;
    public:
        DeclareVar(int name, Type* type);
        Type* type();
        int name();
        virtual void show();
    };

    class DeleteVar : public Token {
    private:
        int name_;
    public:
        DeleteVar(int name);
        int name();
        virtual void show();
    };

    class Goto : public Token {
    private:
        int address_;
    public:
        Goto(int address = 0);
        int address();
        virtual void show();
    };

    class FalseGoto : public Token {
    private:
        int address_;
    public:
        FalseGoto(int address = 0);
        int address();
        virtual void show();
    };

    class ForGoto : public Token {
    private:
        int address_;
    public:
        ForGoto(int address = 0);
        int address();
        virtual void show();
    };

    class ReturnGoto : public Token {
    public:
        ReturnGoto();
    };

    class Var : public Token {
    private:
        int name_;
    public:
        Var(int name);
        int name();
        virtual void show();
    };

    class FuncCall : public Token {
    private:
        int address_;
        int args_num_;
    public:
        FuncCall(int address, int args_num);
        int address();
        int argsNum();
        virtual void show();
    };

    class GetArg : public Token {
        private:
            int name_;
    public:
        GetArg(int name);
        int name();
        virtual void show();
    };

    class DelArgCnt : public Token {
    public:
        DelArgCnt();
    };

    class Print : public Token {
    public:
        Print();
    };

    class Input : public Token {
    public:
        Input();
    };

    class Kill : public Token {
    public:
        Kill();
    };

    class DeclareIterator : public Token {
    public:
        DeclareIterator();
    };

    class SetIteratorVar : public Token {
    private:
        int name_;
    public:
        SetIteratorVar(int name);
        int name();
        virtual void show();
    };

    class MoveIterator : public Token {
    public:
        MoveIterator();
    };

    class DeleteIterator : public Token {
    public:
        DeleteIterator();
    };

    class WhenCompare : public Token {
    public:
        WhenCompare();
    };

    class Operation : public Token {
    private:
        OperationEnum type_;
    public:
        Operation(std::string operation);
        OperationEnum type();
        virtual void show();
    };

    class Const : public Token {
    private:
        Data::Data* data_;
    public:
        Const(Data::Data* data);
        Data::Data* data();
        virtual void show();
    };

    class End : public Token {
    public:
        End();
    };

    class CreateArray : public Token {
    public:
        CreateArray();
    };

    class PushArray : public Token {
    public:
        PushArray();
    };

    class PopArray : public Token {
    public:
        PopArray(); 
    };

    class SizeArray : public Token {
    public:
        SizeArray();
    };

    class SetIt : public Token {
    public:
        SetIt();
    };

    class DelIt : public Token {
    public:
        DelIt();
    };

    class Cast : public Token {
    private:
        Type* type_;
    public:
        Cast(Type* type);
        Type* type();
        virtual void show();
    };

    class Copy : public Token {
    public:
        Copy();
    };

    class PushInit : public Token {
    public:
        PushInit();
    };
};





#endif // !RPN_TOKEN
