#include "Blockqueue.hpp"
#include "Task.hpp"
#include <time.h>
#include <unistd.h>

template<class C, class S>
class BlockQueues {
public:
    BlockQueue<CalTask>* calbq;
    BlockQueue<SavTask>* savbq;
};


int myMath(int x, int y, char op) {
    int ret = 0;
    switch (op) {
    case '+':
        ret = x + y;
        break;
    case '-':
       ret = x - y;
        break;
    case '*':
        ret = x * y;
        break;
    case '/':
        {
            if (y == 0) {
                cerr << "div zero error" << endl;
                ret = -1;
            }
            else {
                ret = x / y;
            }
        }
        break;
    case '%':
        {
            if (y == 0) {
                cerr << "mod zero error" << endl;
                ret = -1;
            }
            else {
                ret = x % y;
            }
        }
        break;
    default:
        // do nothing
        break;
    }
    return ret;
}


void save(const string& message) {
    // 放在文件中：
    const string file = "./log.txt";
    FILE* fp = fopen(file.c_str(), "a+");
    if(fp) {
        fputs(message.c_str(), fp);
        fputs("\n", fp);
        fclose(fp);
    }
    else {
        cerr << "file open error!" << endl;
        return;
    }
}

void* Savor(void* args) {
    // saving:
    BlockQueue<SavTask>* savbq = static_cast<BlockQueues<CalTask, SavTask>*>(args)->savbq;
    while(true) {
        SavTask s;
        savbq->pop(&s);
        s();
        std::cout << "save finished!" << std::endl;
    }
    return nullptr;
}

void* Consumer(void* args) {
    BlockQueue<CalTask>* calbq = static_cast<BlockQueues<CalTask, SavTask>*>(args)->calbq;
    BlockQueue<SavTask>* savbq = static_cast<BlockQueues<SavTask, SavTask>*>(args)->savbq;
    while(true) {
        // Consumer
        CalTask t;
        calbq->pop(&t);

        SavTask s(t.Caltostring_c(), save);
        savbq->push(s);
        std::cout << "Consumer:" << t.Caltostring_c() << "..." << std::endl;
        std::cout << "Task saving ..." << std::endl;
        sleep(1);
    }
    return nullptr;
}

void* Productor(void* args) {
    BlockQueue<CalTask>* bq = static_cast<BlockQueues<CalTask, SavTask>*>(args)->calbq;
    while(true) {
        // Productor
        string oper = "+-*/%";
        int x = rand() % 10 + 1;
        int y = rand() % 50 + 1;
        int code = rand() % oper.size();
        CalTask t(x, y, oper[code], myMath);
        bq->push(t);
        std::cout << "Productor:" << t.Caltostring_p() << "..." << std::endl;
        sleep(1);
    }
    return nullptr;
}


/**
 * 创建三个进程:
 * 1.生产者
 * 2.消费者
 * 3.保存者，用于把这些历史生产消费记录保存在文件中
 */

 /**
  * 生产者消费者高效之处？
  * 怎么支持多消费者、多生产者？
  * 信号量：是一把计数器，衡量临界资源中资源数量多少的计数器
  * 
  */

// 互斥量及初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
// pthread_mutex_init(&mutex, nullptr);

int glo_val = 1000;

void* a_thread(void* args) {
    // cout << "a thread..." << endl;
    while(true) {
        pthread_mutex_lock(&mutex);
        glo_val = 200;
        cout << "a:" << glo_val<< endl;
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

void* b_thread(void* args) {
    // cout << "b thread..." << endl;
    while(true) {
        pthread_mutex_lock(&mutex);
        glo_val = 100;
        cout << "b:" << glo_val<< endl;
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    /*
    srand((unsigned long)time(nullptr));
    BlockQueues<CalTask, SavTask> bqs;
    bqs.calbq = new BlockQueue<CalTask>();
    bqs.savbq = new BlockQueue<SavTask>();

    pthread_t p, c, s;

    pthread_create(&p, nullptr, Productor, (void*)&bqs);
    pthread_create(&c, nullptr, Consumer, (void*)&bqs);
    pthread_create(&s, nullptr, Savor, (void*)&bqs);

    pthread_join(p, nullptr);
    pthread_join(c, nullptr);
    pthread_join(s, nullptr);
    

    delete bqs.calbq;
    delete bqs.savbq;
    */
    


    pthread_t a, b;
    pthread_create(&a, nullptr, a_thread, (void*)"a");
    pthread_create(&a, nullptr, b_thread, (void*)"b");
    /**
     * 互斥量？通过 lock/unlock 控制多个线程对共享资源的访问，保证一次只有一个线程操作资源。（用来保证同步性，保护共享资源）
     * 实际的可能存在的情况是：一份公共资源可能允许访问不同的区域
     * 所以我们先申请信号量，从而提前知道临界资源的使用情况，故而信号量必须是公共资源
     * 信号量是计数器，递增/递减，且必须保证原子性
     * sem_t sem: 
     * 1、++ 释放资源  V 操作
     * 2、-- 申请资源  P 操作
     * 核心：PV原语
     * 
     * */

    

    pthread_join(a, nullptr);
    sleep(1);
    pthread_join(b, nullptr);

    pthread_mutex_destroy(&mutex);

    return 0;
}


