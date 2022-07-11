#include "call_ids.h"
FuncId::FuncId(std::string name, std::vector<Type*> args) : name_(name), args_(args) {}

bool FuncId::operator< (const FuncId& other) const {
    if (name_ != other.name_) return name_ < other.name_;
    if (args_.size() != other.args_.size()) return args_.size() < other.args_.size();
    for (int i = 0; i < args_.size(); ++i) {
        if ((*args_[i]) != (*other.args_[i])) return *args_[i] < *other.args_[i];
    }
    return false;
}

int FuncId::argsNum() { return args_.size(); }

std::string FuncId::name() { return name_; }


MethodId::MethodId(Type* method_type, std::string name, std::vector<Type*> args) :
    method_type_(method_type), name_(name), args_(args) {}

bool MethodId::operator<(const MethodId& other) const {
    if (*(method_type_) != *(other.method_type_)) return *method_type_ < *other.method_type_;
    if (name_ != other.name_) return name_ < other.name_;
    if (args_.size() != other.args_.size()) return args_.size() < other.args_.size();
    for (int i = 0; i < args_.size(); ++i) {
        if ((*args_[i]) != (*other.args_[i])) return *args_[i] < *other.args_[i];
    }
    return false;
}

int MethodId::argsNum() { return args_.size(); }

std::string MethodId::name() { return name_; }

Type* MethodId::methodType() { return method_type_; }