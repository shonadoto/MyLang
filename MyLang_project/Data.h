#ifndef DATA
#define DATA
#include "SemanticsAnalyzer.h"

#pragma once

namespace Data {

    enum class DataEnum {
        BOOL, CHAR, INT, FLOAT, RANGE, STRING, ARRAY
    };

    class Data {
    private:
        DataEnum type_;
    public:
        Data(DataEnum type);
        DataEnum type();
        virtual void show();
        virtual void print();
        virtual void input();
        virtual bool in_range(int64_t ind);
        virtual void pop();
        virtual int size();
        virtual bool is_zero();
        virtual bool less_zero();
        virtual bool more_zero();

        virtual ~Data();
    };

    class Bool : public Data {
    private:
        bool data_;
    public:
        Bool(bool data = false);
        Bool(const Bool& other);
        Bool(Data* other);
        void set(bool data);
        bool get();
        virtual void show();
        virtual void print();
        virtual void input();
        virtual bool is_zero();
        virtual bool less_zero();
        virtual bool more_zero();

        bool operator==(const Bool& other);

        virtual ~Bool();
    };

    class Char : public Data {
    private:
        char data_;
    public:
        Char(char data = 0);
        Char(const Char& other);
        Char(Data* other);
        void set(char data);
        char get();
        virtual void show();
        virtual void print();
        virtual void input();
        virtual bool is_zero();
        virtual bool less_zero();
        virtual bool more_zero();

        bool operator==(const Char& other);

        virtual ~Char();
    };

    class Int : public Data {
    private:
        int64_t data_;
    public:
        Int(int64_t data = 0);
        Int(const Int& other);
        Int(Data* other);
        void set(int64_t data);
        int64_t get();
        virtual void show();
        virtual void print();
        virtual void input();
        virtual bool is_zero();
        virtual bool less_zero();
        virtual bool more_zero();

        bool operator==(const Int& other);

        virtual ~Int();
    };

    class Float : public Data {
    private:
        double data_;
    public:
        Float(double data = 0);
        Float(const Float& other);
        Float(Data* other);
        void set(double data);
        double get();
        virtual void show();
        virtual void print();
        virtual void input();
        virtual bool is_zero();
        virtual bool less_zero();
        virtual bool more_zero();

        bool operator==(const Float& other);

        virtual ~Float();
    };

    class String : public Data {
    private:
        std::string data_;
    public:
        String(std::string data = "");
        String(const String& other);
        String(Data* other);
        void set(std::string data);
        void set(int64_t ind, char value);
        std::string get();
        char get(int64_t ind);
        bool in_range(int64_t ind);
        int size();
        virtual void pop();
        virtual void show();
        virtual void print();
        virtual void input();

        bool operator==(const String& other);

        virtual ~String();
    };

    class Range : public Data {
    private:
        int64_t from_, to_, step_;
    public:
        Range(int64_t from = 0, int64_t to = 0, int64_t step = 0);
        Range(const Range& other);
        Range(Data* other);
        void set(int64_t from, int64_t to, int64_t step);
        int64_t from();
        int64_t to();
        int64_t step();
        int64_t get(int64_t ind);
        bool in_range(int64_t ind);
        virtual void show();
        virtual void print();

        bool operator==(const Range& other);

        virtual ~Range();
    };

    class Array : public Data {
    private:
        std::vector<Data*> data_;
    public:
        Array(std::vector<Data*> data = std::vector<Data*>(0));
        Array(const Array& other);
        Array(Data* other);
        void set(std::vector<Data*> data);
        std::vector<Data*> get();
        int size();
        Data* get(int64_t ind);
        void set(int64_t ind, Data* value);
        bool in_range(int64_t ind);
        virtual void pop();
        virtual void show();
        virtual void print();

        bool operator==(const Array& other);

        virtual ~Array();
    };

};

#endif // !DATA

