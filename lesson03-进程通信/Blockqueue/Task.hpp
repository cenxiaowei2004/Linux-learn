#pragma once

#include <iostream>
#include <functional>

using namespace std;

class CalTask {
public:
    using func_t = std::function<int(int, int, char)>;

    CalTask(int _x, int _y, char _op, func_t _func) : x(_x), y(_y), op(_op), func(_func) {}
    CalTask() {}
    
    string Caltostring_c() {
        char buffer[64];
        int ret = func(x, y, op);
        snprintf(buffer, sizeof buffer, "%d %c %d = %d", x,op, y, ret);
        return buffer;
    }

    string Caltostring_p() {
        char buffer[64];
        int ret = func(x, y, op);
        snprintf(buffer, sizeof buffer, "%d %c %d = ?", x, op, y);
        return buffer;
    }


private:
    std::string operatorstr = "+-*/%";
    int x;
    int y;
    char op;
    func_t func;
};

class SavTask {
public:
    using func_t = function<void(const string&)>;
    SavTask() {}
    SavTask(const string& _message, func_t _funct) : message(_message), funct(_funct) {}

    void operator()() {
        funct(message);
    }
private:
    string message;
    func_t funct;
};