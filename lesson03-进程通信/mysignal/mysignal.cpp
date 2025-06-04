#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <signal.h>

void Usage() {
    std::cout << "Usage: ./Mysignal pid signo" << std::endl;
}

// int main(int argc, char *argv[]) {
//     if(argc != 3) {
//         Usage();
//         exit(1);
//     }
//     pid_t pid = atoi(argv[1]);
//     int signo = atoi(argv[2]);
//     int n = kill(pid, signo);
//     return 0;
// }

int main() {
    while(1) {
        pid_t pid = getpid();
        std::cout << pid << "Hello World!" << std::endl;
        sleep(1);
    }
    return 0;
}