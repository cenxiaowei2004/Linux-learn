#pragma once

#include <fstream>
#include <iostream>

class util {
public:
    static std::string getFirstLine(std::string& inbuffer, const std::string& sep) {
        size_t pos = inbuffer.find(sep);
        return inbuffer.substr(0, pos);
    }

    static bool readFile(const std::string path, std::string* out) {
        string line;
        ifstream in(path, ios::binary);
        if (!in.is_open()) {
            return false;
        }
        while (getline(in, line)) {
            *out += line;
        }
        in.close();
        return true;
    }
};
