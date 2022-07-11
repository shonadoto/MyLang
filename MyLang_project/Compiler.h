#ifndef  COMPILER
#define COMPILER

#pragma once

#include "Executor.h"

class Compiler
{
private:
    Path* compiler_path_;
    Path* file_;
    LecsicalAnalyzer* lecser_;
    SyntaxAnalyzer* syntax_;
    SemanticsAnalyzer* sema_;
    RpnCreator* rpn_creator_;
    Executor* executor_;
    std::set<std::string> imported_;
public:
    Compiler(Path* source_file, Path* compiler_path);
    void workLecsical();
    void workSyntax();
    void workSema();
    void workRpnCreator();
    void workExecutor();
};

#endif //  COMPILER
