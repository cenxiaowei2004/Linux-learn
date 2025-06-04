#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>

int main(int argc, char *argv[]) {
    while(true) {
        std::cout << "一段运行的进程" << getpid() << ":print Hello World!" << std::endl;
        sleep(2);
    }
    return 0;
}