#ifndef PATH
#define PATH


#pragma once
#include "includes.h"

class Path {
private:
    std::string name_;
    Path* parent_;

public:
    Path(std::string full_path, bool raw = 0);
    Path(std::string file_name, Path* dir);
    Path* parent();
    std::string fullPath(int raw = 1);
};

#endif // !PATH

