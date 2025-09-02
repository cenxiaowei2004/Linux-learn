#pragma once

#include <stdarg.h>
#include <unistd.h>
#include <cstdarg>
#include <iostream>

#include <time.h>

#define NORMAL 1
#define DEBUG 2
#define WARNING 3
#define ERROR 4
#define FATAL 5

using namespace std;

char info[1024];

const char* getLevel(int level) {
    switch (level) {
        case 1:
            return "NORMAL";
        case 2:
            return "DEBUG";
        case 3:
            return "WARNING";
        case 4:
            return "ERROR";
        case 5:
            return "FATAL";
        default:
            return "UNKNOWN BUG";
    }
}

char* getTime() {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    snprintf(info, sizeof(info), "%d-%d-%d", 1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday);
    return info;
}

void logMessage(int level, const char* format, ...) {
    // 参数得压栈顺序：从右向左
    // va_list start;
    // va_start(start);
    // // va_arg:向后移动指定类型
    // va_arg(start, int);
    // // va_end:start变为nullptr
    // va_end(start);

    // 格式:[日志等级][时间戳][pid][message]
    char logContent[1024];
    char logPrefix[1024];

    snprintf(logPrefix, sizeof(logPrefix), "[%s][%s][%d]", getLevel(level), getTime(), getpid());
    va_list arg;
    va_start(arg, format);

    // 示例：[NORMAL][2025-9-2][2419023]threadPool init success
    vsnprintf(logContent, sizeof(logContent), format, arg);
    cout << logPrefix << logContent << endl;
}
