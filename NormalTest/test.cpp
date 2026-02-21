#include <
#include <iostream>
using namespace std;

int main() {
    // 获取客户端的版本信息
    cout << "mysql client version: " << mysql_get_client_info() << endl;
    return 0;
}

// https://downloads.mysql.com/archives/c-c/