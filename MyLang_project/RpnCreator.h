#ifndef RPN_CREATOR
#define RPN_CREATOR
#include "RpnToken.h"

#pragma once
class RpnCreator
{
private:
    class Tid;

    std::vector<LecsicalToken> tokens_;
    std::vector<RPN::Token*> rpn_;
    int cur_token_;
    int cur_rpn_;
    std::queue<MethodId> method_calls_, mc_copy;
    std::queue<FuncId> function_calls_, fc_copy;
    std::map<FuncId, int> funcs_;
    std::map<MethodId, int> methods_;
    std::map<std::string, int> var_nums_;
    std::stack<std::stack<int>> breaks_, continues_;
    std::queue<int> function_calls_rpn_, method_calls_rpn_;
    Tid* cur_tid_;
    int prev_global_var;

    int main_start;
public:
    RpnCreator(std::vector<LecsicalToken> tokens,
        std::queue<FuncId> function_calls,  std::queue<MethodId> method_calls);
    void work();
    void debug();
    std::vector<RPN::Token*> getTokens();
    int getMain();
    int varNumber();
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

    void pushRpn(RPN::Token* token);
    void setRpn(int it, RPN::Token* token);
    void nextTid(bool is_loop = false, bool is_function = false);
    void prevTid();

    int getPrior(std::string);
    bool isRightAssoc(std::string);

    void setCalls();
    void setBreaks();
    void setContinues(int address);

    Type* TYPE();
    void FUNCTION_DECLARATION();
    void METHOD_DECLARATION();
    std::pair<Type*, bool> FUNCTION_ARGUMENT();
    void VARIABLE_DECLARATION();

    void COMPOUND_OPERATOR();
    void NON_COMPOUND_OPERATOR();
    void OPERATOR();
    void DERIVED_OPERATOR();
    void IF();
    void WHILE();
    void DO_WHILE();
    void FOR();
    void WHEN();

    void EXPRESSION();
    void FUNCTION_CALL();
    void VARIABLE();
    void CONST_BOOL();
    void CONST_CHAR();
    void CONST_FLOAT();
    void CONST_INT();
    void CONST_RANGE();
    void CONST_STRING();
    void INITIALIZER_LIST();
    void METHOD_CALL();
    void INDEX_CALL();
    void CAST();
};


#endif // !RPN_CREATOR
