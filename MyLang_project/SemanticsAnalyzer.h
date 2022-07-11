#ifndef SEMANTICS_ANALYZER
#define SEMANTICS_ANALYZER
#include "call_ids.h"

#pragma once
class SemanticsAnalyzer
{
private:
    class SemanticsError;
    class TID;

    std::vector<LecsicalToken> tokens_;
    int cur_token_;
    int analysis_stage_; // 0 - funcitions, 1 - other, 2 - end
    std::map<FuncId, Type*> funcs_;
    std::map<MethodId, Type*> methods_;
    std::queue<MethodId> method_calls_;
    std::queue<FuncId> function_calls_;
    Type* cur_func_type_;
    int returns_cnt_;
    int loop_cnt_;
    bool in_method_;
    TID* tid_;
public:
    SemanticsAnalyzer(std::vector<LecsicalToken> tokens);
    std::vector<LecsicalToken> getTokens();
    std::queue<MethodId> getMethodCalls();
    std::queue<FuncId> getFunctionCalls();
    void work();

private:
    LecsicalToken token();
    std::string tokenName();
    LecsicalEnum tokenType();
    std::string tokenFile();
    int tokenLine();
    int tokenChar();
    void tokenNext();
    void tokenPrev();
    bool isType(LecsicalToken token);

    bool goodCast(Type* first, Type* second);

    Type* operate(Type* left, std::string operation, Type* right);
    Type* operate(Type* left, std::string operation);

    void nextTID();
    void prevTID();

    Type* TYPE();
    void FUNCTION_DECLARATION();
    void VARIABLE_DECLARATION();
    void METHOD_DECLARATION();
    std::pair<Type*, bool> FUNCTION_ARGUMENT();

    void COMPOUND_OPERATOR();
    void NON_COMPOUND_OPERATOR();
    void OPERATOR();
    void DERIVED_OPERATOR();
    void IF();
    void WHILE();
    void DO_WHILE();
    void FOR();
    void WHEN();

    Type* EXPRESSION();
    Type* VALUE();
    Type* CAST();
    Type* INITIALIZER_LIST();
    Type* METHOD_CALL(Type* var_type);
    Type* FUNCTION_CALL();

    Type* PRIORITY_1();
    Type* PRIORITY_2();
    Type* PRIORITY_3();
    Type* PRIORITY_4();
    Type* PRIORITY_5();
    Type* PRIORITY_6();
    Type* PRIORITY_7();
    Type* PRIORITY_8();
    Type* PRIORITY_9();
    Type* PRIORITY_10();
    Type* PRIORITY_11();
    Type* PRIORITY_12();
    Type* PRIORITY_13();
    Type* PRIORITY_14();
    Type* PRIORITY_15();
    Type* PRIORITY_16();
};

#endif // !SEMANTICS_ANALYZER