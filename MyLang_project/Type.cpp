#include "Type.h"

Type::Type(TypeEnum base_type, Type* parent, bool is_var) : base_type_(base_type), parent_(parent), is_var_(is_var) {}

Type::Type(std::string type, Type* parent, bool is_var) {
    is_var_ = is_var;
    parent_ = parent;
    base_type_ = TypeEnum::VOID;
    if (type == "bool") base_type_ = TypeEnum::BOOL;
    if (type == "char") base_type_ = TypeEnum::CHAR;
    if (type == "int") base_type_ = TypeEnum::INT;
    if (type == "float") base_type_ = TypeEnum::FLOAT;
    if (type == "range") base_type_ = TypeEnum::RANGE_3;
    if (type == "string") base_type_ = TypeEnum::STRING;
    if (type == "array") base_type_ = TypeEnum::ARRAY;
}

Type::Type(Type* other) {
    is_var_ = other->is_var_;
    base_type_ = other->base_type_;
    parent_ = NULL;
    if (other->parent_ != NULL)
        parent_ = new Type(other->parent_);
}

Type::Type(const Type& other) : base_type_(other.base_type_) {
    is_var_ = other.is_var_;
    if (other.parent_ == NULL) {
        parent_ = NULL;
    }
    else {
        parent_ = new Type(*other.parent_);
    }
}

Type::Type(Type&& other) : base_type_(other.base_type_) {
    is_var_ = other.is_var_;
    if (other.parent_ == NULL) {
        parent_ = NULL;
    }
    else {
        parent_ = new Type(*other.parent_);
    }
}

Type::~Type() {
    delete parent_;
}

Type::TypeEnum Type::baseType() { return base_type_; }

Type* Type::parent() { return parent_; }

void Type::setIsVar(bool is_var) { is_var_ = is_var; }

bool Type::isVar() { return is_var_; }

std::string Type::toString() const {
    if (parent_ == NULL) {
        switch (base_type_)
        {
        case TypeEnum::BOOL:
            return "bool";
            break;
        case TypeEnum::CHAR:
            return "char";
            break;
        case TypeEnum::INT:
            return "int";
            break;
        case TypeEnum::FLOAT:
            return "float";
            break;
        case TypeEnum::STRING:
            return "string";
            break;
        case TypeEnum::RANGE_2:
            return "range2";
            break;
        case TypeEnum::RANGE_3:
            return "range3";
            break;
        case TypeEnum::ARRAY:
            return "array";
            break;
        case TypeEnum::VOID:
            return "void";
            break;
        default:
            return "NONE";
            break;
        }
    }
    return "array<" + parent_->toString() + ">";
}

bool Type::operator==(const Type& other) const {
    if (base_type_ != other.base_type_) return false;
    if (parent_ == NULL && other.parent_ == NULL) return true;
    if (parent_ == NULL || other.parent_ == NULL) return false;
    return *parent_ == *(other.parent_);
}

bool Type::operator!=(const Type& other) const {
    return !this->operator==(other);
}

bool Type::operator<(const Type& other) const {
    if (base_type_ == other.base_type_) {
        if (parent_ != NULL && other.parent_ != NULL) {
            return *parent_ < *other.parent_;
        }
        if (parent_ == NULL)
            return true;
        if (other.parent_ == NULL)
            return false;
    }
    return base_type_ < other.base_type_;
}