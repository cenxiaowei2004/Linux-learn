#pragma once

#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

class Task {
    using func_t = std::function<int(int, int, char)>;
    // typedef std::function<int(int,int)> func_t;
public:
    Task() {}
    Task(int x, int y, char op, func_t func)
        : _x(x), _y(y), _op(op), _callback(func) {}
    std::string operator()() {
        int result = _callback(_x, _y, _op);
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "%d %c %d = %d", _x, _op, _y, result);
        return buffer;
    }
    std::string toTaskString() {
        char buffer[1024];
        snprintf(buffer, sizeof buffer, "%d %c %d = ?", _x, _op, _y);
        return buffer;
    }

private:
    int _x;
    int _y;
    char _op;
    func_t _callback;
};

const std::string oper = "+-*/%";

int mymath(int x, int y, char op) {
    switch (op) {
        case '+':
            return x + y;
        case '-':
            return x - y;
        case '*':
            return x * y;
        case '/':
            if (y == 0)
                throw std::invalid_argument("Division by zero");
            return x / y;
        case '%':
            if (y == 0)
                throw std::invalid_argument("Modulo by zero");
            return x % y;
        default:
            throw std::invalid_argument("Invalid operator");
    }
}