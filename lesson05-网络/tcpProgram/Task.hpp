#pragma once

#include "log.hpp"

#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

void DataIO(int sock) {
    char buffer[1024];
    while (true) {
        // 读
        ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
        if (n == 0) {
            logMessage(NORMAL, "client quit");
            break;
        }
        buffer[n] = 0;
        cout << "receive message:" << buffer << endl;
        ;
        string response = buffer;
        response += "[server send]";
        // 写
        write(sock, response.c_str(), response.size());
    }
    close(sock);
}

class Task {
    using func_t = std::function<void(int)>;

public:
    Task() {}
    Task(int _sock, func_t _func = DataIO) : sock(_sock), callback(_func) {}
    void operator()() {
        callback(sock);
    }

private:
    int sock;
    func_t callback;
};
