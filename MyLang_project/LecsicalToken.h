
#ifndef LECSICAL_TOKEN
#define LECSICAL_TOKEN

#pragma once
#include "Path.h"

enum class LecsicalEnum
{
    // terminal states for StateMachine

    NONE, RESERVED, NAME, OPERATION, COLON, SEMI_COLON, COMMA, POINT, SQUARE_BRACE, CURLY_BRACE, ROUND_BRACE, SPACE,
    CONST_BOOL, CONST_CHAR, CONST_INT, CONST_FLOAT, CONST_STRING, CONST_RANGE,
    DEFINE, IFNDEF, ENDIF,
    COMMENT, INT_POINT
};

class LecsicalToken
{
private:
    std::string name_;
    LecsicalEnum type_;
    std::string file_name_; // to-do
    int line_number_, char_number_;

public:

    LecsicalToken() : name_(""), file_name_(""), line_number_(0), char_number_(0), type_(LecsicalEnum::NONE) {}

    LecsicalToken(std::string name, LecsicalEnum type, int line_number, int char_number, std::string file_name) :
        name_(name),
        type_(type),
        line_number_(line_number),
        char_number_(char_number),
        file_name_(file_name)
    {}
    
    LecsicalToken(const LecsicalToken& other) :
        name_(other.name_),
        type_(other.type_),
        line_number_(other.line_number_),
        char_number_(other.char_number_),
        file_name_(other.file_name_)
    {}
    
    LecsicalToken(LecsicalToken&& other) noexcept :
        name_(std::move(other.name_)),
        type_(std::move(other.type())),
        line_number_(std::move(other.line_number_)),
        char_number_(std::move(other.char_number_)),
        file_name_(std::move(other.file_name_))
    {}

    LecsicalToken& operator=(const LecsicalToken& other) {
        name_ = other.name_;
        file_name_ = other.file_name_;
        line_number_ = other.line_number_;
        char_number_ = other.char_number_;
        type_ = other.type_;
        return *this;
    }

    void tokenDebug();
    std::string name();
    int lineNumber();
    int charNumber();
    LecsicalEnum type();
    std::string file_name();
    void setName(std::string name);
};



#endif // LECSICAL_TOKEN
