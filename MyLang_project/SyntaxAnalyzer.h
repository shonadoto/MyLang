#ifndef SYNTAX_ANALYZER
#define SYNTAX_ANALYZER
#pragma once

#include "LecsicalAnalyzer.h"

class SyntaxAnalyzer
{
    /*
        Description:
        Checks whether the program conforms to the grammatics of the language
    */

private:
    class SyntaxError;

    Path* file_;
    std::vector<LecsicalToken> tokens_;
    int cur_token_;
    std::set<std::string>& imported_;

public:
    SyntaxAnalyzer(std::vector<LecsicalToken> tokens, Path* file, std::set<std::string>& imported);

    std::vector<LecsicalToken> getTokens();
    void debugTokens();

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

    void importFile(std::string source);
    //void deleteDefines();

    void NAME();

    void IMPORT();

    void TYPE();
    void FUNCTION_DECLARATION();
    void VARIABLE_DECLARATION();
    void METHOD_DECLARATION();
    void FUNCTION_ARGUMENT();
    void COMPOUND_OPERATOR();
    void OPERATOR();
    void DERIVED_OPERATOR();
    void NON_COMPOUND_OPERATOR();
    void IF();
    void WHILE();
    void DO_WHILE();
    void FOR();
    void WHEN();

    void EXPRESSION();
    void VALUE();
    void CAST();
    void INITIALIZER_LIST();
    void METHOD_CALL();
    void FUNCTION_CALL();

    void PRIORITY_1();
    void PRIORITY_2();
    void PRIORITY_3();
    void PRIORITY_4();
    void PRIORITY_5();
    void PRIORITY_6();
    void PRIORITY_7();
    void PRIORITY_8();
    void PRIORITY_9();
    void PRIORITY_10();
    void PRIORITY_11();
    void PRIORITY_12();
    void PRIORITY_13();
    void PRIORITY_14();
    void PRIORITY_15();
    void PRIORITY_16();

};


#endif // SYNTAX_ANALYZER
