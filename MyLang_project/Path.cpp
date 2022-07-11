#include "Path.h"

Path::Path(std::string full_path, bool raw) {
    name_ = "";
    while (full_path.size() != 0 && *(full_path.end() - 1) != '\\') {
        name_ += *(full_path.end() - 1);
        full_path.pop_back();
    }
    std::reverse(name_.begin(), name_.end());
    if (full_path.size() == 0) {
        parent_ = NULL;
        return;
    }
    if (!raw)
        full_path.pop_back();
    full_path.pop_back();
    parent_ = new Path(full_path, raw);
}

Path::Path(std::string file_name, Path* dir) {
    name_ = file_name;
    parent_ = dir;
}

Path* Path::parent() { return parent_; }

std::string Path::fullPath(int raw) {
    if (parent_ == NULL)
        return name_;
    return parent_->fullPath(raw) + (raw ? "\\" : "\\\\") + name_;
}