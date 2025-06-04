#include "comm.hpp"

int main() {
    std::cout << "client" << std::endl;
    key_t key = getKey();
    std::cout << "0x" << key << std::endl;
    int shmid = getShm(key);
    std::cout << "shmid = " << shmid << std::endl;

    // 关联
    sleep(3);
    std::cout << "attach success!" << std::endl;
    char* start = (char*)attachShm(shmid);

    const char* msg = "message from cilent";
    int cnt = 1;
    while(true) {
        snprintf(start, MAX_SIZE, "%s[消息编号：%d]", msg, cnt++);
        sleep(1);
    }
    // 去关联
    sleep(3);   
    std::cout << "attach exit!" << std::endl;
    detchShm(start);
     
    return 0;
}