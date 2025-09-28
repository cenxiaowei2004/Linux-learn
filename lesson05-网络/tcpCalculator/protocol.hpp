#pragma once

#include <json/json.h>
#include <cstring>
#include <iostream>
#include <string>

#include <sys/socket.h>
#include "log.hpp"

#define SEP " "
#define SEP_LEN strlen(SEP)
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

// Request & Response

enum { SUCCESS = 0, DIV_ZERO, MOD_ZERO, OP_ERR };

// 协议定制：
// 为了每次都读到一个报文
// 增加长度头部，形成完整报文："num1 op num2\r\n" -> "content_len\r\n""num1 op num2\r\n"
string addLength(const string& text) {
    string send_string = to_string(text.size()) + LINE_SEP + text + LINE_SEP;
    return send_string;
}

// 去除长度头部："content_len\r\n""num1 op num2\n\r" -> "num1 op num2\n\r"
bool delLength(string& text, string* out) {
    int pos = text.find(LINE_SEP);
    if (pos == string::npos)
        return false;
    int content_len = stoi(text.substr(0, pos));
    *out = text.substr(pos + LINE_SEP_LEN, content_len);
    return true;
}

// 从socket接收完整报文（解决TCP粘包问题），保证获取到完整的报文:
// "content_len\r\n""num1 op num2\n\r""content_len\r\n""num1 op num2\n\r"
bool recvRequset(int sock, string& inbuffer, string* out) {
    char buffer[1024];
    while (true) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n == 0)
            return false;
        else {
            buffer[n] = 0;
            inbuffer += buffer;
            int pos = inbuffer.find(LINE_SEP);
            if (pos == string::npos)
                continue;
            string text_len_string = inbuffer.substr(0, pos);
            int text_len = stoi(text_len_string);
            int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len;
            if (inbuffer.size() < total_len)
                continue;

            // for debug:
            // cout << "处理前的缓冲区:\n" << inbuffer << endl;
            // 至少有一个完整的报文
            *out = inbuffer.substr(0, total_len);
            inbuffer.erase(0, total_len);
            // cout << "处理后的缓冲区:\n" << inbuffer << endl;
            break;
        }
    }
    return true;
}

// 网络计算器
class Request {
public:
    void serialize(string* out) {
#ifdef MySelf
        // "num1 op num2"
        string num1_string = to_string(num1);
        string num2_string = to_string(num2);
        *out = num1_string + SEP + op + SEP + num2_string;
#else
        // Json
        Json::Value root;
        root["num1"] = num1;
        root["num2"] = num2;
        root["op"] = op;

        Json::StyledWriter writer;
        *out = writer.write(root);

#endif
    }

    // "num1 op num2\n\r"
    bool deserialize(const string& in) {
#ifdef MySelf
        auto left = in.find(SEP);
        auto right = in.rfind(SEP);
        // 异常处理

        if (left == string::npos || right == string::npos || left == right)
            return false;
        string num1_string = in.substr(0, left);
        string num2_string = in.substr(right + SEP_LEN);

        op = in[left + SEP_LEN];
        num1 = stoi(num1_string);
        num2 = stoi(num2_string);
#else
        // Json
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        num1 = root["num1"].asInt();
        num2 = root["num2"].asInt();
        op = root["op"].asInt();
#endif
        return true;
    }

public:
    Request() {}
    Request(int _num1, int _num2, char _op) : num1(_num1), num2(_num2), op(_op) {}

    int num1;
    int num2;
    char op;
};

class Response {
public:
    Response() {}
    Response(int _exitcode, int _result) : exitcode(_exitcode), result(_result) {}

    void serialize(string* out) {
#ifdef MySelf

        string exitcode_string = to_string(exitcode);
        string result_string = to_string(result);

        *out = exitcode_string + SEP + result_string;
#else
        // Json
        Json::Value root;
        root["exitcode"] = exitcode;
        root["result"] = result;

        Json::StyledWriter writer;
        *out = writer.write(root);

#endif
    }

    bool deserialize(string& in) {
#ifdef MySelf

        int pos = in.find(SEP);
        if (pos == string::npos)
            return false;
        string exitcode_string = in.substr(0, pos);
        string result_string = in.substr(pos + SEP_LEN, in.size());
        if (result_string.empty() || exitcode_string.empty())
            return false;
        exitcode = stoi(exitcode_string);
        result = stoi(result_string);
#else
        // Json
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        exitcode = root["exitcode"].asInt();
        result = root["result"].asInt();
#endif
        return true;
    }

    int exitcode;  // 0表示成功，!0表示出错
    int result;
};