#include "udpServer.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Server;

const string dictFile = "./dict.txt";
unordered_map<string, string> dict;

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

void dataHandle(string clientIp, uint16_t clientPort, string message) {
    cout << "deal with data" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        Usage();
        exit(USAGE_ERR);
    }

    initDict();

    uint16_t port = atoi(argv[1]);
    std::unique_ptr<udpServer> server(new udpServer(port, dataHandle));
    server->initServer();
    server->start();

    return 0;
}
