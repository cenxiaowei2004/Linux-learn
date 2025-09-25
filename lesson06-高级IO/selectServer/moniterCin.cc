
#include <iostream>

#include <poll.h>

#include "log.hpp"

void moniterCin() {
    struct pollfd pfd = {0, POLLIN, 0};
    nfds_t nfd = 1;

    while (true) {
        int n = poll(&pfd, nfd, 3000);
        if (n < 0) {
            logMessage(WARNING, "poll error");
            continue;
        } else if (n == 0) {
            logMessage(NORMAL, "poll timeout");
            continue;
        } else if (pfd.revents == POLLIN) {
            logMessage(NORMAL, "cin... ");
        }
    }
}

int main() {
    moniterCin();
    return 0;
}