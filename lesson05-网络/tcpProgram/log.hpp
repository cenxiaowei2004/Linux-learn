
#include <iostream>

#define NORMAL 1
#define DEBUG 2
#define WARNING 3
#define ERROR 4
#define FATAL 5

using namespace std;

void logMessage(int level, string message) {
    // 格式:[日志等级][时间戳][pid][message]
    cout << "log:" << message << endl;
}
