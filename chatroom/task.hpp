#pragma once

#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

class task {
    using func_t = std::function<void(int)>;

public:
    task() {}
    task(int _sock, func_t _func) : sock(_sock), callback(_func) {}
    void operator()() { callback(sock); }

private:
    int sock;
    func_t callback;
};
