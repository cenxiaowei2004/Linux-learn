#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>


#define PATHNAME "."

#define PROJ_ID 0x66
#define MAX_SIZE 1000

// 获key值
key_t getKey() {
    key_t key = ftok(PATHNAME, PROJ_ID);
    if(key < 0) {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }
    return key;
}

int getShmHelper(key_t key, int flags) {
    int shmid = shmget(key, MAX_SIZE ,flags | 0666);
    if(shmid < 0) {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }
    return shmid;
}

int getShm(key_t key) {
    return getShmHelper(key, IPC_CREAT);
}

// 创建共享空间
int createShm(key_t key) {
    return getShmHelper(key, IPC_CREAT | IPC_EXCL);
}

// 释放共享空间
void deleteShm(int shmid) {
    if(shmctl(shmid, IPC_RMID, nullptr) == -1) {
        std::cout << errno << strerror(errno) << std::endl;
        exit(1);
    }
}

// 关联共享空间
void* attachShm(int shmid) {
    void* mem = shmat(shmid, nullptr, 0);
    if(mem == (void*)-1) {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }
    return mem;
}

// 去关联共享空间
void detchShm(void* start) {
    if(shmdt(start) == -1) {
        std::cout << "detchshm error" << std::endl;
        return;
    }
}



