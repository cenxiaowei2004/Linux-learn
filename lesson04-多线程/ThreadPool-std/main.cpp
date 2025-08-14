#include <unistd.h>
#include <memory>
#include "Task.hpp"
#include "ThreadPool.hpp"

int main() {
    ThreadPool<Task>::getInstance()->run();

    int x, y;
    char op;
    while (1) {
        std::cout << "请输入数据1# ";
        std::cin >> x;
        std::cout << "请输入数据2# ";
        std::cin >> y;
        std::cout << "请输入你要进行的运算#";
        std::cin >> op;
        Task t(x, y, op, mymath);

        ThreadPool<Task>::getInstance()->push(t);
        sleep(1);
    }
}
