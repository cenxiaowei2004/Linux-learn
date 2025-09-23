
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include "util.hpp"

using namespace std;

void execTasks() {
    cout << "exec other task..." << endl;
}

/*
int main() {
    // 设置 0 号文件描述符为非阻塞
    setNonBlock(0);
    char buffer[1024];
    while (true) {
        cout << ">>> ";
        fflush(stdout);
        size_t n = read(0, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n - 1] = 0;
            cout << "echo# " << buffer << endl;
        } else if (n == 0) {
            cout << "buffer end# " << endl;
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cout << "unavailable data";
            } else if (errno == EINTR) {
                continue;
            } else {
                cout << strerror(errno) << endl;
                break;
            }
        }
        sleep(1);
        execTasks();
    }
    return 0;
}
*/

int main() {
    setNonBlock(0);
    char buffer[1024];
    while (true) {
        std::cout << ">>> ";
        ssize_t size = read(0, buffer, sizeof(buffer) - 1);
        if (size < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << strerror(errno) << std::endl;
                sleep(1);
                execTasks();
                continue;
            } else if (errno == EINTR) {
                std::cout << strerror(errno) << std::endl;
                sleep(1);
                execTasks();
                continue;
            } else {
                std::cout << strerror(errno) << std::endl;
                break;
            }
        }
        if (size == 0) {
            std::cout << "buffer end# " << std::endl;
            break;
        }
        buffer[size - 1] = 0;
        std::cout << "echo# " << buffer << std::endl;
        execTasks();
        // select()
    }
    return 0;
}
