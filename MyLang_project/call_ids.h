#ifndef  CALL_IDS
#define CALL_DS

#pragma once

#include "Type.h"

class FuncId;
class MethodId;

class FuncId {
private:
    std::string name_;
    std::vector<Type*> args_;
public:
    FuncId(std::string name, std::vector<Type*> args);
    bool operator< (const FuncId& other) const;
    int argsNum();
    std::string name();
};

class MethodId {
private:
    Type* method_type_;
    std::string name_;
    std::vector<Type*> args_;
public:
    MethodId(Type* method_type, std::string name, std::vector<Type*> args);
    bool operator<(const MethodId& other) const;
    int argsNum();
    std::string name();
    Type* methodType();
};

#endif // ! CALL_IDS
