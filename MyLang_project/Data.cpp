#include "Data.h"

Data::Data::Data(DataEnum type) : type_(type) {}

Data::Data::~Data() { type_ = DataEnum(0); }

Data::DataEnum Data::Data::type() { return type_; }

void Data::Data::show() {
    std::cout << "Data. Type: ";
    switch (type_)
    {
    case DataEnum::BOOL:
        std::cout << "bool";
        break;
    case DataEnum::CHAR:
        std::cout << "char";
        break;
    case DataEnum::INT:
        std::cout << "int";
        break;
    case DataEnum::FLOAT:
        std::cout << "float";
        break;
    case DataEnum::RANGE:
        std::cout << "range";
        break;
    case DataEnum::STRING:
        std::cout << "string";
        break;
    case DataEnum::ARRAY:
        std::cout << "array";
        break;
    default:
        std::cout << "WTF";
        break;
    }
}

void Data::Data::print() {}

void Data::Data::input() {}

bool Data::Data::in_range(int64_t ind) { return false; }

void Data::Data::pop() {}

int Data::Data::size() { return 0; }

bool Data::Data::is_zero() { return true; }

bool Data::Data::less_zero() { return false; }

bool Data::Data::more_zero() { return false; }



// Bool
Data::Bool::Bool(bool data) : data_(data), Data(DataEnum::BOOL) {}

Data::Bool::Bool(const Bool& other) : data_(other.data_), Data(DataEnum::BOOL) {}

Data::Bool::Bool(Data* other) : Data(DataEnum::BOOL) {
    Bool* tmp = static_cast<Bool*>(other);
    data_ = tmp->data_;
}

Data::Bool::~Bool() { data_ = 0; }

void Data::Bool::set(bool data) { data_ = data; }

bool Data::Bool::get() { return data_; }

void Data::Bool::show() {
    std::cout << "Bool: " << (data_ ? "true" : "false");
}

void Data::Bool::print() { std::cout << (data_ ? "true" : "false"); }

void Data::Bool::input() { std::cin >> data_; }

bool Data::Bool::operator==(const Bool& other) {
    return data_ == other.data_;
}

bool Data::Bool::is_zero() { return data_ == 0; }

bool Data::Bool::less_zero() { return data_ < 0; }

bool Data::Bool::more_zero() { return data_ > 0; }




// Char
Data::Char::Char(char data) : data_(data), Data(DataEnum::CHAR) {}

Data::Char::Char(const Char& other) : data_(other.data_), Data(DataEnum::CHAR) {}

Data::Char::Char(Data* other) : Data(DataEnum::CHAR) {
    Char* tmp = static_cast<Char*>(other);
    data_ = tmp->data_;
}

Data::Char::~Char() { data_ = 0; }

void Data::Char::set(char data) { data_ = data; }

char Data::Char::get() {
    return data_;
}

void Data::Char::show() {
    std::cout << "Char: " << data_;
}

void Data::Char::print() { std::cout << data_; }

void Data::Char::input() { std::cin >> data_; }

bool Data::Char::operator==(const Char& other) {
    return data_ == other.data_;
}

bool Data::Char::is_zero() { return data_ == 0; }

bool Data::Char::less_zero() { return data_ < 0; }

bool Data::Char::more_zero() { return data_ > 0; }


// Int
Data::Int::Int(int64_t data) : data_(data), Data(DataEnum::INT) {}

Data::Int::Int(const Int& other) : data_(other.data_), Data(DataEnum::INT) {}

Data::Int::Int(Data* other) : Data(DataEnum::INT) {
    Int* tmp = static_cast<Int*>(other);
    data_ = tmp->data_;
}

Data::Int::~Int() { data_ = 0; }

void Data::Int::set(int64_t data) { data_ = data; }

int64_t Data::Int::get() { return data_; }

void Data::Int::show() {
    std::cout << "Int: " << data_;
}

void Data::Int::print() { std::cout << data_; }

void Data::Int::input() { std::cin >> data_; }

bool Data::Int::operator==(const Int& other) {
    return data_ == other.data_;
}

bool Data::Int::is_zero() { return data_ == 0; }

bool Data::Int::less_zero() { return data_ < 0; }

bool Data::Int::more_zero() { return data_ > 0; }


//Float
Data::Float::Float(double data) : data_(data), Data(DataEnum::FLOAT) {}

Data::Float::Float(const Float& other) : data_(other.data_), Data(DataEnum::FLOAT) {}

Data::Float::Float(Data* other) : Data(DataEnum::FLOAT) {
    Float* tmp = static_cast<Float*>(other);
    data_ = tmp->data_;
}

Data::Float::~Float() { data_ = 0; }

void Data::Float::set(double data) { data_ = data; }

double Data::Float::get() { return data_; }

void Data::Float::show() {
    std::cout << "Float: " << data_;
}

void Data::Float::print() { std::cout << data_; }

void Data::Float::input() { std::cin >> data_; }

bool Data::Float::operator==(const Float& other) {
    return data_ == other.data_;
}

bool Data::Float::is_zero() { return data_ == 0; }

bool Data::Float::less_zero() { return data_ < 0; }

bool Data::Float::more_zero() { return data_ > 0; }



// String
Data::String::String(std::string data) : data_(data), Data(DataEnum::STRING) {}

Data::String::String(const String& other) : data_(other.data_), Data(DataEnum::STRING) {}

Data::String::String(Data* other) : Data(DataEnum::STRING) {
    String* tmp = static_cast<String*>(other);
    data_ = tmp->data_;
}

Data::String::~String() { data_ = ""; }

void Data::String::set(std::string data) { data_ = data; }

void Data::String::set(int64_t ind, char value) {
    if (ind == data_.size()) {
        data_ += value;
        return;
    }
    if (ind >= 0 && ind < data_.size()) {
        data_[ind] = value;
        return;
    }
    throw "String set shit";
}

std::string Data::String::get() { return data_; }

char Data::String::get(int64_t ind) { return data_[ind]; }

bool Data::String::in_range(int64_t ind) { return ind >= 0 && ind < data_.size(); }

int Data::String::size() { return data_.size(); }

void Data::String::pop() { data_.pop_back(); }

void Data::String::show() {
    std::cout << "String: " << data_;
}

void Data::String::print() { std::cout << data_; }

void Data::String::input() { std::cin >> data_; }

bool Data::String::operator==(const String& other) {
    return data_ == other.data_;
}



//Range
Data::Range::Range(int64_t from, int64_t to, int64_t step) :
    from_(from), to_(to), step_(step), Data(DataEnum::RANGE) {}

Data::Range::Range(const Range& other) : from_(other.from_), to_(other.to_), step_(other.step_), Data(DataEnum::RANGE) {}

Data::Range::Range(Data* other) : Data(DataEnum::RANGE) {
    Range* tmp = static_cast<Range*>(other);
    from_ = tmp->from_;
    to_ = tmp->to_;
    step_ = tmp->step_;
}

Data::Range::~Range() { from_ = 0, to_ = 0, step_ = 0; }

void Data::Range::set(int64_t from, int64_t to, int64_t step) { from_ = from, to_ = to, step_ = step; }

int64_t Data::Range::from() { return from_; }

int64_t Data::Range::to() { return to_; }

int64_t Data::Range::step() { return step_; }

int64_t Data::Range::get(int64_t ind) {
    return from_ + step_ * ind;
}

bool Data::Range::in_range(int64_t ind) {
    int max = (to_ - from_) / step_;
    return ind <= max && ind >= 0;
}

void Data::Range::show() {
    std::cout << "Range: " << from_ << " " << to_ << " " << step_;
}

void Data::Range::print() {
    std::cout << "range(" << from_ << ", " << to_ << ", " << step_ << ")";
}

bool Data::Range::operator==(const Range& other) {
    return from_ == other.from_ && to_ == other.to_ && step_ == other.step_;
}



// Array
Data::Array::Array(std::vector<Data*> data) : data_(data), Data(DataEnum::ARRAY) {}

Data::Array::Array(const Array& other) : Data(DataEnum::ARRAY) {
    for (auto i : other.data_) {
        DataEnum type = i->type();
        Data* push;
        switch (type)
        {
        case DataEnum::BOOL:
            push = new Bool(i);
            break;
        case DataEnum::CHAR:
            push = new Char(i);
            break;
        case DataEnum::INT:
            push = new Int(i);
            break;
        case DataEnum::FLOAT:
            push = new Float(i);
            break;
        case DataEnum::RANGE:
            push = new Range(i);
            break;
        case DataEnum::STRING:
            push = new String(i);
            break;
        case DataEnum::ARRAY:
            push = new Array(i);
            break;
        default:
            break;
        }
        data_.push_back(push);
    }
}

Data::Array::Array(Data* other) : Data(DataEnum::ARRAY) {
    Array* tmp = static_cast<Array*>(other);
    for (auto i : tmp->data_) {
        DataEnum type = i->type();
        Data* push;
        switch (type)
        {
        case DataEnum::BOOL:
            push = new Bool(i);
            break;
        case DataEnum::CHAR:
            push = new Char(i);
            break;
        case DataEnum::INT:
            push = new Int(i);
            break;
        case DataEnum::FLOAT:
            push = new Float(i);
            break;
        case DataEnum::RANGE:
            push = new Range(i);
            break;
        case DataEnum::STRING:
            push = new String(i);
            break;
        case DataEnum::ARRAY:
            push = new Array(i);
            break;
        default:
            break;
        }
        data_.push_back(push);
    }
}

Data::Array::~Array() {
    for (auto i : data_) delete i;
    data_.resize(0);
}

void Data::Array::set(std::vector<Data*> data) {
    data_.clear();
    for (auto i : data) {
        DataEnum type = i->type();
        Data* push;
        switch (type)
        {
        case DataEnum::BOOL:
            push = new Bool(i);
            break;
        case DataEnum::CHAR:
            push = new Char(i);
            break;
        case DataEnum::INT:
            push = new Int(i);
            break;
        case DataEnum::FLOAT:
            push = new Float(i);
            break;
        case DataEnum::RANGE:
            push = new Range(i);
            break;
        case DataEnum::STRING:
            push = new String(i);
            break;
        case DataEnum::ARRAY:
            push = new Array(i);
            break;
        default:
            break;
        }
        data_.push_back(push);
    }
}

std::vector<Data::Data*> Data::Array::get() { return data_; }

Data::Data* Data::Array::get(int64_t ind) { return data_[ind]; }

void Data::Array::set(int64_t ind, Data* value) {
    if (ind == data_.size()) {
        data_.push_back(value);
        return;
    }
    if (ind >= 0 && ind <= data_.size()) {
        data_[ind] = value;
        return;
    }
    throw "Array set shit";
}

bool Data::Array::in_range(int64_t ind) {
    return ind >= 0 && ind < data_.size();
}

int Data::Array::size() { return data_.size(); }

void Data::Array::show() {
    std::cout << "Array: {" << std::endl;
    for (int i = 0; i < data_.size(); ++i) {
        std::cout << "\t";
        data_[i]->show();
        if (i != data_.size() - 1)
            std::cout << "," << std::endl;
    }
    std::cout << "}" << std::endl;
}

void Data::Array::print() {
    std::cout << "{";
    for (int i = 0; i < data_.size(); ++i) {
        if (data_[i]->type() == DataEnum::CHAR)
            std::cout << '\'';
        else if (data_[i]->type() == DataEnum::STRING)
            std::cout << '\"';

        data_[i]->print();

        if (data_[i]->type() == DataEnum::CHAR)
            std::cout << '\'';
        else if (data_[i]->type() == DataEnum::STRING)
            std::cout << '\"';
        if (i != data_.size() - 1)
            std::cout << ", ";
    }
    std::cout << "}";
}

bool Data::Array::operator==(const Array& other) {
    if (data_.size() != other.data_.size()) return false;
    for (int i = 0; i < data_.size(); ++i) {
        DataEnum type = data_[i]->type();
        bool eqls = false;
        switch (type)
        {
        case DataEnum::BOOL:
        {
            Bool* f = static_cast<Bool*>(data_[i]);
            Bool* s = static_cast<Bool*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::CHAR:
        {
            Char* f = static_cast<Char*>(data_[i]);
            Char* s = static_cast<Char*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::INT:
        {
            Int* f = static_cast<Int*>(data_[i]);
            Int* s = static_cast<Int*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::FLOAT:
        {
            Float* f = static_cast<Float*>(data_[i]);
            Float* s = static_cast<Float*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::RANGE:
        {
            Range* f = static_cast<Range*>(data_[i]);
            Range* s = static_cast<Range*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::STRING:
        {
            String* f = static_cast<String*>(data_[i]);
            String* s = static_cast<String*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        case DataEnum::ARRAY:
        {
            Array* f = static_cast<Array*>(data_[i]);
            Array* s = static_cast<Array*>(other.data_[i]);
            eqls = *f == *s;
        }
        break;
        default:
            break;
        }
        if (!eqls) return false;
    }
    return true;
}

void Data::Array::pop() {
    delete data_[data_.size() - 1];
    data_.pop_back();
}