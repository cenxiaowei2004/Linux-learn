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
// 增加长度头部："num1 op num2" -> "content_len\r\n""num1 op num2\r\n"
string enLength(const string& text) {
    string send_string = to_string(text.size()) + LINE_SEP + text + LINE_SEP;
    return send_string;
}

// 去除长度头部："content_len\r\n""num1 op num2\r\n" -> "num1 op num2"
bool deLength(string& text, string* out) {
    int pos = text.find(LINE_SEP);
    if (pos == string::npos)
        return false;
    int content_len = stoi(text.substr(0, pos));
    *out = text.substr(pos + LINE_SEP_LEN, content_len);
    return true;
}

// 解析报文，保证有一个完整的："content_len\r\n""num1 op num2\n\r""content_len\r\n""num1 op num2\n\r"
bool parsePackage(string& inbuffer, string* out) {
    int pos = inbuffer.find(LINE_SEP);
    if (pos == string::npos)
        return false;
    string text_len_string = inbuffer.substr(0, pos);
    int text_len = stoi(text_len_string);
    int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len;
    // 还没形成一个完整报文
    if (inbuffer.size() < total_len)
        return false;
    *out = inbuffer.substr(0, total_len);
    inbuffer.erase(0, total_len);
    return true;
}

class Request {
public:
    // 序列化和反序列化：json方案
    void serialize(string* out) {
        Json::Value root;
        root["num1"] = num1;
        root["num2"] = num2;
        root["op"] = op;

        Json::StyledWriter writer;
        *out = writer.write(root);
    }

    // "num1 op num2\n\r"
    bool deserialize(const string& in) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        num1 = root["num1"].asInt();
        num2 = root["num2"].asInt();
        op = root["op"].asInt();

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
        Json::Value root;
        root["exitcode"] = exitcode;
        root["result"] = result;

        Json::StyledWriter writer;
        *out = writer.write(root);
    }

    bool deserialize(string& in) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);

        exitcode = root["exitcode"].asInt();
        result = root["result"].asInt();

        return true;
    }

    int exitcode;  // 0表示成功，!0表示出错
    int result;
};