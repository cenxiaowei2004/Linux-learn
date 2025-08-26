#include "udpServer.hpp"
#include "onlineUser.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Server;

const string dictFile = "./dict.txt";
unordered_map<string, string> dict;

onlineUser onlineuser;

void Usage() {
    std::cout << "Usage form: ./udpServer port" << std::endl;
}

bool cutString(const string& s, string* k, string* v, const string& sep) {
    auto pos = s.find(sep);
    if (pos == string::npos) {
        return false;
    }
    *k = s.substr(0, pos);
    *v = s.substr(pos + sep.size());

    return true;
}

void initDict() {
    string line;
    string key, value;
    ifstream in(dictFile, ios::binary);
    if (!in.is_open()) {
        cerr << "file open error:" << errno << ":" << strerror(errno) << endl;
        exit(OPEN_ERR);
    }
    while (getline(in, line)) {
        if (cutString(line, &key, &value, ":")) {
            dict.insert(make_pair(key, value));
        }
    }
    in.close();
}

// translate program
void dataHandle(int fd, string clientIp, uint16_t clientPort, string message) {
    // cout << "deal with data" << endl;
    string response;
    auto iter = dict.find(message);
    if (iter == dict.end())
        response = "unknown";
    else
        response = iter->second;

    // 向 client 返回结果
    sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(clientIp.c_str());
    client.sin_port = htons(clientPort);
    sendto(fd, response.c_str(), response.size(), 0, (sockaddr*)&client, sizeof(client));
}

// mini shell
void handleCommand(int fd, string clientIp, uint16_t clientPort, string cmd) {
    string response;
    char buffer[1024];
    FILE* file = popen(cmd.c_str(), "r");
    if (file == nullptr)
        response = cmd + "fail!";

    while (fgets(buffer, sizeof(buffer), file)) {
        response += buffer;
    }
    pclose(file);

    // 向 client 返回结果
    sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(clientIp.c_str());
    client.sin_port = htons(clientPort);
    sendto(fd, response.c_str(), response.size(), 0, (sockaddr*)&client, sizeof(client));
}

// chat program
void routeMessage(int fd, string clientIp, uint16_t clientPort, string message) {
    if (message == "online")
        onlineuser.addUser(clientIp, clientPort);
    if (message == "offline")
        onlineuser.delUser(clientIp, clientPort);

    if (onlineuser.isOnline(clientIp, clientPort)) {
        onlineuser.broadcastMessage(fd, clientIp, clientPort, message);

    } else {
        // 向 client 返回结果
        string response = "[未上线，请运行:online]";
        sockaddr_in client;
        client.sin_family = AF_INET;
        client.sin_addr.s_addr = inet_addr(clientIp.c_str());
        client.sin_port = htons(clientPort);
        sendto(fd, response.c_str(), response.size(), 0, (sockaddr*)&client, sizeof(client));
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        Usage();
        exit(USAGE_ERR);
    }

    // initDict();

    uint16_t port = atoi(argv[1]);

    // std::unique_ptr<udpServer> server(new udpServer(port, handleCommand));
    std::unique_ptr<udpServer> server(new udpServer(port, routeMessage));

    server->initServer();
    server->start();

    return 0;
}
