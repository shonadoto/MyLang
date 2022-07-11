#include "Compiler.h"

Compiler::Compiler(Path* file, Path* compiler_path) : 
    file_(file), compiler_path_(compiler_path), lecser_(NULL), syntax_(NULL), sema_(NULL), rpn_creator_(NULL), executor_(NULL) {
}

void Compiler::workLecsical() {
    lecser_ = new LecsicalAnalyzer(file_);
    lecser_->createTokens();
}

void Compiler::workSyntax() {
    std::vector<LecsicalToken> tokens = lecser_->getTokens();
    tokens.insert(tokens.begin(), LecsicalToken("\"" + compiler_path_->fullPath(0) + "\\\\standart_lib.mylang" + "\"", LecsicalEnum::CONST_STRING, 0, 0, file_->fullPath()));
    tokens.insert(tokens.begin(), LecsicalToken("import", LecsicalEnum::RESERVED, 0, 0, file_->fullPath()));
    syntax_ = new SyntaxAnalyzer(tokens, file_, imported_);
    syntax_->work();
    // syntax_->debugTokens();
}

void Compiler::workSema() {
    sema_ = new SemanticsAnalyzer(syntax_->getTokens());
    sema_->work();
}

void Compiler::workRpnCreator() {
    rpn_creator_ = new RpnCreator(sema_->getTokens(), sema_->getFunctionCalls(), sema_->getMethodCalls());
    rpn_creator_->work();
    //rpn_creator_->debug();
}

void Compiler::workExecutor() {
    executor_ = new Executor(rpn_creator_->getMain(),rpn_creator_->varNumber(), rpn_creator_->getTokens());
    executor_->work();
}