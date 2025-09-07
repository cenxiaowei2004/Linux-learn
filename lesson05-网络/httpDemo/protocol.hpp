#pragma once

#include <json/json.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "log.hpp"
#include "util.hpp"

enum { SUCCESS = 0, DIV_ZERO, MOD_ZERO, OP_ERR };

#define SEP "\r\n"

const string default_path = "./wwwroot";
const string home_path = "index.html";
const string not_found_path = "wwwroot/404.html";

class HttpRequest {
public:
    HttpRequest() {}

    void parse() {
        string line = util::getFirstLine(inbuffer, SEP);

        std::stringstream ss(line);
        ss >> method >> url >> version;
        path = default_path + url;
        if (path[path.size() - 1] == '/')
            path += home_path;
    }

    string inbuffer;

    string method;
    string url;
    string version;
    string path;
};

class HttpResponse {
public:
    HttpResponse() {}

    string outbuffer;
};
