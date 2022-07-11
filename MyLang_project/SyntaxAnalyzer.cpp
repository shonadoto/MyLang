#include "SyntaxAnalyzer.h"


SyntaxAnalyzer::SyntaxAnalyzer(std::vector<LecsicalToken> tokens, Path* file, std::set<std::string>& imported) :
    tokens_(tokens), file_(file), imported_(imported) {
    cur_token_ = 0;
    if (imported_.find(file_->fullPath()) != imported.end()) {
        tokens_.clear();
    }
    else {
        imported.insert(file_->fullPath());
    }

}

class SyntaxAnalyzer::SyntaxError {
private:
    std::string expected_;
    LecsicalToken token_;

public:
    SyntaxError(std::string expected, LecsicalToken got) : expected_(expected), token_(got) {

    }
    void sendError() {

        std::cout << "File: " << token_.file_name() << "\n";
        std::cout << "Line: " << token_.lineNumber() << ", character: " << token_.charNumber() << ".\n";

        std::cout << "Syntax error: expected " << expected_;
        if (token_.type() != LecsicalEnum::NONE)
            std::cout << ", got \"" << token_.name() << "\".";
        else
            std::cout << ".";
        std::cout << "\n\n";
    }
};

LecsicalToken SyntaxAnalyzer::token() { return tokens_[cur_token_]; }
std::string SyntaxAnalyzer::tokenName() { return tokens_[cur_token_].name(); }
LecsicalEnum SyntaxAnalyzer::tokenType() { return tokens_[cur_token_].type(); }
void SyntaxAnalyzer::tokenNext() { cur_token_++; }
void SyntaxAnalyzer::tokenPrev() { cur_token_--; }
std::vector<LecsicalToken> SyntaxAnalyzer::getTokens() { return tokens_; }
std::string SyntaxAnalyzer::tokenFile() { return tokens_[cur_token_].file_name(); }
int SyntaxAnalyzer::tokenLine() { return tokens_[cur_token_].lineNumber(); }
int SyntaxAnalyzer::tokenChar() { return tokens_[cur_token_].charNumber(); }

bool SyntaxAnalyzer::isType(LecsicalToken token) {
    return token.name() == "bool" || token.name() == "char" ||
        token.name() == "int" || token.name() == "float" || token.name() == "string" || token.name() == "range" || token.name() == "array";
}

void SyntaxAnalyzer::debugTokens() {
    for (auto& token : tokens_) {
        token.tokenDebug();
    }
}


void SyntaxAnalyzer::importFile(std::string source) {

    // delete " from edges of string
    source.erase(source.begin());
    source.erase(source.end() - 1);
    Path* import_path;
    if (source.find(":") != std::string::npos)
        import_path = new Path(source);
    else
        import_path = new Path(source, file_->parent());

    // lexicaly analyze imported file
    LecsicalAnalyzer lexer(import_path);
    lexer.createTokens();
    std::vector<LecsicalToken> import_tokens = lexer.getTokens();

    // sintaxicaly analyze imported file
    SyntaxAnalyzer import_synt(import_tokens, import_path, imported_);
    import_synt.work();
    import_tokens = import_synt.getTokens();

    //import_tokens.insert(import_tokens.begin(), LecsicalToken(source, LecsicalEnum::DEFINE, -1, -1, source));
    //import_tokens.insert(import_tokens.begin(), LecsicalToken(source, LecsicalEnum::IFNDEF, -1, -1, source));
    //import_tokens.push_back(LecsicalToken(source, LecsicalEnum::ENDIF, -1, -1, source));
    // delete import tokens from original file and insert inported tokens
    tokenPrev();
    tokens_.erase(tokens_.begin() + cur_token_, tokens_.begin() + cur_token_ + 2);
    tokens_.insert(tokens_.begin() + cur_token_, import_tokens.begin(), import_tokens.end());
    cur_token_ += import_tokens.size();
}

/*
void SyntaxAnalyzer::deleteDefines() {
    std::set<std::string> defined;
    std::stack<bool> to_save;
    std::queue<LecsicalToken> tokens_queue;
    for (auto& t : tokens_) tokens_queue.push(t);
    tokens_.clear();
    to_save.push(true);
    while (!tokens_queue.empty()) {
        LecsicalToken cur = tokens_queue.front();
        if (cur.type() == LecsicalEnum::IFNDEF) {

            // if previous ifn/endif-block is good and ifn is true we take current block
            if (to_save.top() && defined.find(cur.name()) == defined.end())
                to_save.push(true);
            else
                to_save.push(false);
        }
        else if (cur.type() == LecsicalEnum::DEFINE) {
            defined.insert(cur.name());
        }
        else if (cur.type() == LecsicalEnum::ENDIF) {
            to_save.pop();
        }
        else if (to_save.top()) {
            tokens_.push_back(cur);
        }
        tokens_queue.pop();
    }
}

*/

void SyntaxAnalyzer::work() {
    try
    {
        if (tokens_.size() == 0) return;
        tokens_.push_back(LecsicalToken("", LecsicalEnum::NONE, tokens_[tokens_.size() - 1].lineNumber(),
            tokens_[tokens_.size() - 1].charNumber(), tokens_[tokens_.size() - 1].file_name()));

        while (tokenType() != LecsicalEnum::NONE) {
            int start = cur_token_;
            if (tokenName() == "import") {
                IMPORT();
            }
            else if (tokenName() == "void") {
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
            else if (isType(token())) {

                

                TYPE();


                if (isType(token())) {
                    cur_token_ = start;
                    METHOD_DECLARATION();
                }
                else {

                    tokenNext(); // from name to (

                    if (tokenName() == "(") {
                        cur_token_ = start; // back to TYPE
                        FUNCTION_DECLARATION();
                    }
                    else {
                        cur_token_ = start;
                        VARIABLE_DECLARATION(); // back to TYPE
                    }
                }
            }
            else {
                throw SyntaxError("import or type", token());
            }
        }
        tokens_.pop_back();

        //deleteDefines();

    }
    catch (SyntaxError error)
    {
        error.sendError();
        throw std::string("");
    }
}

void SyntaxAnalyzer::NAME() {
    if (tokenType() != LecsicalEnum::NAME) throw SyntaxError("name", token());
    tokenNext();
}

// IMPORT

void SyntaxAnalyzer::IMPORT() {
    tokenNext(); // from import to CONST_STRING
    if (tokenType() != LecsicalEnum::CONST_STRING) throw SyntaxError("source", token());
    importFile(tokenName());
}


// DECLARATIONS

void SyntaxAnalyzer::TYPE() {
    if (isType(token()) && tokenName() != "array") {

        tokenNext(); // from basic types to smth
        return;
    }
    else if (tokenName() == "array") {

        // array

        tokenNext(); // from array to <

        if (tokenName() != "<") throw SyntaxError(">", token());
        tokenNext(); // from < to TYPE

        TYPE();

        if (tokenName() != ">") throw SyntaxError(">", token());
        tokenNext(); // from > to smth
    }
    else {
        throw SyntaxError("type", token());
    }
}

void SyntaxAnalyzer::FUNCTION_DECLARATION() {

    if (tokenName() != "void")
        TYPE(); // from type to name
    else
        tokenNext(); // from void to name

    NAME(); // from name to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to func_arg or )

    if (tokenName() != ")") {
        while (true) {
            FUNCTION_ARGUMENT(); // from func_arg to , or )

            if (tokenName() == ",") {
                tokenNext(); // from , to func_arg
            }
            else if (tokenName() == ")") {
                break;
            }
            else {
                throw SyntaxError(", or )", token());
            }
        }
    }
    tokenNext(); // from ) to compound
    COMPOUND_OPERATOR(); // from compound to smth
}

void SyntaxAnalyzer::METHOD_DECLARATION() {

    if (tokenName() != "void")
        TYPE(); // from type to type
    else
        tokenNext(); // from void to typw
    TYPE(); // from type to .
    tokenNext(); // from . to name
    NAME(); // from name to (



    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to ) or func_arg

    if (tokenName() != ")") {
        while (true) {
            FUNCTION_ARGUMENT(); // from func_arg to ,

            if (tokenName() == ",") {
                tokenNext(); // from , to func_arg
            }
            else if (tokenName() == ")") {
                break;
            }
            else {
                throw SyntaxError(")", token());
            }
        }
    }
    tokenNext(); // from ) to {
    COMPOUND_OPERATOR(); // from compound to smth
}

void SyntaxAnalyzer::FUNCTION_ARGUMENT() {
    TYPE(); // from type to name
    NAME(); // from name to smth
    if (tokenName() == "=") {
        tokenPrev(); // from = to expression
        EXPRESSION(); // from expression to smth
    }
}

void SyntaxAnalyzer::VARIABLE_DECLARATION() {
    TYPE(); // from type to name

    while (true) {

        NAME(); // from name to = or , or ;
        if (tokenName() == "=") {
            tokenNext(); // from = to expression
            EXPRESSION(); // from expression to smth
        }
        if (tokenName() == ";")
            break;
        else if (tokenName() == ",") {
            tokenNext(); // from , to name
            continue;
        }
        else
            throw SyntaxError(";", token());
    }
    tokenNext(); //from ; to smth

}

// OPERATORS

void SyntaxAnalyzer::COMPOUND_OPERATOR() {
    if (tokenName() != "{") throw SyntaxError("{", token());
    tokenNext();

    while (tokenName() != "}") {
        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to smth
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth
        }
        if (tokenType() == LecsicalEnum::NONE) 
            throw SyntaxError("}", token());
    }

    tokenNext(); // from } to smth
}

void SyntaxAnalyzer::OPERATOR() {
    if (tokenName() == "break") {
        tokenNext(); // from break to ;
        if (tokenName() != ";")
            throw SyntaxError(";", token());
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "continue") {
        tokenNext(); // from continue to ;
        if (tokenName() != ";") 
            throw SyntaxError(";", token());
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "return") {
        tokenNext(); // from return to expression;
        if (tokenName() == ";") {
            tokenNext(); // from ; to smth
        }
        else {
            EXPRESSION(); // from expression to ;
            if (tokenName() != ";") 
                throw SyntaxError(";", token());
            tokenNext(); // from ; to smth
        }
    }
    else if (tokenName() == "print") {
        tokenNext(); // from print to (
        if (tokenName() != "(") 
            throw SyntaxError("(", token());
        
        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            EXPRESSION(); // from expression to , or )
            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
            else
                throw SyntaxError(")", token());
        }
        tokenNext(); // from ) to ;
        if (tokenName() != ";")
            throw SyntaxError(";", token());
        tokenNext(); // from ; to smth
    }
    else if (tokenName() == "input") {
        tokenNext(); // from input to smth
        if (tokenName() != "(")
            throw SyntaxError("(", token());

        while (true) {
            tokenNext(); // from ( or , to smth
            if (tokenName() == ")")
                break;
            EXPRESSION(); // from expression to , or )
            if (tokenName() == ",")
                continue;
            else if (tokenName() == ")")
                break;
            else
                throw SyntaxError(")", token());
        }
        tokenNext(); // from ) to ;
        if (tokenName() != ";")
            throw SyntaxError(";", token());
        tokenNext(); // from ; to smth
    }
    else {
        throw SyntaxError("operator", token());
    }
}

void SyntaxAnalyzer::DERIVED_OPERATOR() {
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
    else {
        throw SyntaxError("derived operator", token());
    }
}

void SyntaxAnalyzer::NON_COMPOUND_OPERATOR() {
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
        if (tokenName() != ";") throw SyntaxError(";", token());
        tokenNext(); // from ; to smth
    }
}

void SyntaxAnalyzer::IF() {
    tokenNext(); // from if to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to expression

    EXPRESSION(); // from expression to )

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

    if (tokenName() == "else") {
        tokenNext(); // from else to compound or derived

        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to smth
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to smth;
        }
    }
}

void SyntaxAnalyzer::WHILE() {
    tokenNext(); // from while to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to expression

    EXPRESSION(); // from expression to )

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth;
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth
    }
}

void SyntaxAnalyzer::DO_WHILE() {
    tokenNext(); // from do to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

    if (tokenName() != "while") throw SyntaxError("while", token());
    tokenNext(); // from while to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to expression

    EXPRESSION();

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to ;

    if (tokenName() != ";") throw SyntaxError(";", token());
    tokenNext(); // from ; to smth

}

void SyntaxAnalyzer::FOR() {
    tokenNext(); // from for to (


    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to type

    TYPE();

    NAME(); // from name to :

    if (tokenName() != ":") throw SyntaxError(":", token());
    tokenNext(); // from : to expression

    EXPRESSION(); // from expression to )

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to {

    if (tokenName() == "{") {
        COMPOUND_OPERATOR(); // from compound to smth
    }
    else {
        NON_COMPOUND_OPERATOR(); // from non-compound to smth;
    }

}

void SyntaxAnalyzer::WHEN() {
    tokenNext(); // from when to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to expression

    EXPRESSION(); // from expression to )

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to {

    if (tokenName() != "{") throw SyntaxError("{", token());
    tokenNext(); // from { to expression or else

    while (tokenName() != "else") {
        EXPRESSION(); // from expression to :

        if (tokenName() != ":") throw SyntaxError(":", token());
        tokenNext(); // from : to 

        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to expression
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to expression
        }
        if (tokenName() == "}") break;
        if (tokenType() == LecsicalEnum::NONE) throw SyntaxError("}", token());

    }

    if (tokenName() == "else") {
        tokenNext(); // from else to :

        if (tokenName() != ":") throw SyntaxError(":", token());
        tokenNext(); // from : to operator


        if (tokenName() == "{") {
            COMPOUND_OPERATOR(); // from compound to }
        }
        else {
            NON_COMPOUND_OPERATOR(); // from non-compound to }
        }
    }

    if (tokenName() != "}") throw SyntaxError("}", token());
    tokenNext(); // from } to smth
}


// EXPRESSION

void SyntaxAnalyzer::EXPRESSION() {
    PRIORITY_16();
}

void SyntaxAnalyzer::VALUE() {

    if (tokenType() == LecsicalEnum::CONST_BOOL || tokenType() == LecsicalEnum::CONST_CHAR ||
        tokenType() == LecsicalEnum::CONST_INT || tokenType() == LecsicalEnum::CONST_FLOAT ||
        tokenType() == LecsicalEnum::CONST_STRING || tokenType() == LecsicalEnum::CONST_RANGE) {
        tokenNext(); // from const to smth
        return;
    }

    if (isType(token())) {
        CAST();
        return;
    }

    if (tokenName() == "{") {
        INITIALIZER_LIST(); // from init_list to smth
        return;
    }

    if (tokenName() == "it") {
        tokenNext(); // from it to smth
        return;
    }
    else {

        if (tokenType() != LecsicalEnum::NAME) throw SyntaxError("value", token());

        NAME(); // from name to smth
        if (tokenName() == "(") {
            tokenPrev(); // from ( to name
            FUNCTION_CALL(); // from function_call to smth
        }
    }
}

void SyntaxAnalyzer::CAST() {
    TYPE(); // from type to (

    if (tokenName() != "(") throw SyntaxError("(", token());
    tokenNext(); // from ( to expression;

    EXPRESSION(); // from expression to )

    if (tokenName() != ")") throw SyntaxError(")", token());
    tokenNext(); // from ) to smth
}

void SyntaxAnalyzer::INITIALIZER_LIST() {
    if (tokenName() != "{") throw SyntaxError("{", token());

    while (tokenName() != "}") {
        tokenNext();
        EXPRESSION(); // from expression to , or }
        if (tokenName() == "}") break;
        else if (tokenName() != ",") throw SyntaxError("}", token());
    }

    tokenNext(); // from } to smth

}
/*
void SyntaxAnalyzer::METHOD_CALL() {
    NAME(); // from name to .
    if (tokenName() != ".") throw SyntaxError(".", token());
    tokenNext(); // from . to name

    NAME(); // from name to (

    if (tokenName() != "(") throw SyntaxError("(", token());

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        EXPRESSION(); // from expression to , or )
        if (tokenName() == ")") break;
        else if (tokenName() != ",") throw SyntaxError(")", token());
    }
    tokenNext(); // from ) to smth
}*/

void SyntaxAnalyzer::METHOD_CALL() {
    if (tokenName() != ".") throw SyntaxError(".", token());
    tokenNext(); // from . to name



    NAME();

    if (tokenName() != "(") throw SyntaxError("(", token());

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        EXPRESSION(); // from expression to , or )
        if (tokenName() == ")") break;
        else if (tokenName() != ",") throw SyntaxError(")", token());
    }
    tokenNext(); // from ) to smth
}

void SyntaxAnalyzer::FUNCTION_CALL() {
    NAME(); // from name to (

    if (tokenName() != "(") throw SyntaxError("(", token());

    while (tokenName() != ")") {
        tokenNext(); // from ( or , to expr
        if (tokenName() == ")") break;

        EXPRESSION(); // from expression to , or )
        if (tokenName() == ")") break;
        else if (tokenName() != ",") throw SyntaxError(")", token());
    }
    tokenNext(); // from ) to smth
}

void SyntaxAnalyzer::PRIORITY_1() {
    if (tokenName() == "(") {
        tokenNext(); // from ( to expr
        EXPRESSION(); // from expression to )
        if (tokenName() != ")") throw SyntaxError(")", token());
        tokenNext(); // from ) to smth
        return;
    }
    VALUE();
}

void SyntaxAnalyzer::PRIORITY_2() {
    PRIORITY_1(); // from p1 to smth
    while (tokenName() == "[" || tokenName() == ".") {
        if (tokenName() == "[") {
            if (tokenName() != "[") throw SyntaxError("[", token());
            tokenNext(); // from [ to expression

            EXPRESSION(); // from expression to ]

            if (tokenName() != "]") throw SyntaxError("]", token());
            tokenNext(); // from ] to smth
        }
        else if (tokenName() == ".") {
            METHOD_CALL();
        }
    }
}

void SyntaxAnalyzer::PRIORITY_3() {
    PRIORITY_2(); // from p2 to smth

    if (tokenName() == "**") {
        tokenNext(); // from ** to p3
        PRIORITY_3(); // from p3 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_4() {
    if (tokenName() == "!" || tokenName() == "~" || tokenName() == "+" || tokenName() == "-") {
        if (tokenName() == "+")
            tokens_[cur_token_].setName("@");
        if (tokenName() == "-")
            tokens_[cur_token_].setName("_");
        tokenNext(); // from !~+- to p4
        PRIORITY_4(); // from p4 to smth
    }
    else {
        PRIORITY_3(); // from p3 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_5() {
    PRIORITY_4(); // from p4 to smth

    if (tokenName() == "*" || tokenName() == "/" || tokenName() == "%") {
        tokenNext(); // from */% to p5
        PRIORITY_5(); // from p5 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_6() {
    PRIORITY_5(); // from p5 to smth

    if (tokenName() == "+" || tokenName() == "-") {
        tokenNext(); // from +- to p6
        PRIORITY_6();
    }
}

void SyntaxAnalyzer::PRIORITY_7() {
    PRIORITY_6(); // from p6 to smth

    if (tokenName() == "<<" || tokenName() == ">>") {
        tokenNext(); // from oper to smth
        PRIORITY_7();
    }
}

void SyntaxAnalyzer::PRIORITY_8() {
    PRIORITY_7(); // from p7 to smth

    if (tokenName() == "<" || tokenName() == ">" || tokenName() == "<=" || tokenName() == ">=") {
        tokenNext(); // from oper to p8
        PRIORITY_8(); // from p8 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_9() {
    PRIORITY_8(); // from p8 to smth

    if (tokenName() == "==" || tokenName() == "!=") {
        tokenNext(); // from oper to p9
        PRIORITY_9(); // from p9 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_10() {
    PRIORITY_9(); // from p9 to smth

    if (tokenName() == "&") {
        tokenNext(); // from oper to p10
        PRIORITY_10(); // from p9 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_11() {
    PRIORITY_10(); // from p10 to smth

    if (tokenName() == "^") {
        tokenNext(); // from oper to p11
        PRIORITY_11(); // from p11 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_12() {
    PRIORITY_11(); // from p11 to smth

    if (tokenName() == "|") {
        tokenNext(); // from oper to p12
        PRIORITY_12(); // from p12 to smth
    }
}


void SyntaxAnalyzer::PRIORITY_13() {
    PRIORITY_12(); // from p13 to smth

    if (tokenName() == "in") {
        tokenNext(); // from oper to p13
        PRIORITY_13(); // from p13 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_14() {
    PRIORITY_13(); // from p13 to smth

    if (tokenName() == "&&") {
        tokenNext(); // from oper to p14
        PRIORITY_14(); // from p14 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_15() {
    PRIORITY_14(); // from p14 to smth

    if (tokenName() == "||") {
        tokenNext(); // from oper to p15
        PRIORITY_15(); // from p15 to smth
    }
}

void SyntaxAnalyzer::PRIORITY_16() {
    PRIORITY_15(); // from p15 to smth

    if (tokenName() == "=" || tokenName() == "+=" || tokenName() == "-=" ||
        tokenName() == "*=" || tokenName() == "/=" || tokenName() == "%=" ||
        tokenName() == "**" || tokenName() == "<<=" || tokenName() == ">>=" ||
        tokenName() == "&=" || tokenName() == "|=" || tokenName() == "^=") {
        tokenNext(); // from oper to p16
        PRIORITY_16(); // from p16 to smth
    }
}
