
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

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

class Task {
private:
    int _x;
    int _y;
    char _op;
    std::function<int(int, int, char)> _callback;

public:
    Task(int x, int y, char op, std::function<int(int, int, char)> callback)
        : _x(x), _y(y), _op(op), _callback(callback) {}

    std::string toTaskString() const { return std::to_string(_x) + " " + _op + " " + std::to_string(_y) + " = ?"; }

    void operator()() {
        int result = _callback(_x, _y, _op);
        std::cout << "result:" + std::to_string(_x) + " " + _op + " " + std::to_string(_y) + " = " +
                         std::to_string(result)
                  << std::endl;
    }
};
