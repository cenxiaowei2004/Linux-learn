#include "comm.hpp"




int main() {

    std::cout << "server" << std::endl;
    key_t key = getKey();
    std::cout << "key = 0x" << key << std::endl;
    // 创建
    // int shmid = createShm(key);
    int shmid = shmget(key, MAX_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    std::cout << "shmid = " << shmid << std::endl;

    // 关联
    std::cout << "attach success!" << std::endl;
    char* start = (char*)attachShm(shmid);
    sleep(10);
    // 使用
    while(true) {
        std::cout << "cilent say:" << start << std::endl;
        sleep(1);
    }


    // 去关联
    std::cout << "attach exit!" << std::endl;
    detchShm(start);
    sleep(10);
    // 释放
    sleep(10);
    deleteShm(shmid);
    return 0;
}

