#ifndef TYPE_CLASS
#define TYPE_CLASS
#pragma once
#include "SyntaxAnalyzer.h"

class Type {
public:
    enum class TypeEnum {
        BOOL, CHAR, INT, FLOAT, STRING, RANGE, ARRAY, VOID
    };
    Type(TypeEnum base_type, Type* parent = NULL, bool is_var = false);
    Type(std::string type = "void", Type* parent = NULL, bool is_var = false);
    Type(Type* other);
    Type(const Type& other);
    Type(Type&& other);
    ~Type();

    TypeEnum baseType();

    Type* parent();

    void setIsVar(bool is_var);

    bool isVar();

    std::string toString() const;

    bool operator==(const Type& other) const;


    bool operator!=(const Type& other) const;

    bool operator<(const Type& other) const;

private:
    TypeEnum base_type_;
    Type* parent_;
    bool is_var_;
};



#endif // !TYPE


