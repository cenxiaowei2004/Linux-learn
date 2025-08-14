#include "Task.hpp"
#include "ThreadPool.hpp"

int main() {
    ThreadPool<Task>* threadPool = new ThreadPool<Task>(5);
    threadPool->run();

    while (true) {
        char op;
        int x, y;
        cout << "请输入数据1#：";
        cin >> x;
        cout << "请输入数据2#：";
        cin >> y;
        cout << "请输入操作符#：";
        cin >> op;
        Task task(x, y, op, mymath);
        threadPool->push(task);
        cout << "completed!" << "current task:" + task.toTaskString() << endl;
        sleep(1);
    }

    return 0;
}