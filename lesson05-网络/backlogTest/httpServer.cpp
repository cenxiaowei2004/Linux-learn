#include "httpServer.hpp"
#include "protocol.hpp"
#include "util.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Server;

void Usage() {
    std::cout << "Usage form: ./calServer port" << std::endl;
}

bool callback(const HttpRequest& req, HttpResponse& resp) {
    cout << "------------------ http request start ----------------------" << endl;
    cout << req.inbuffer;
    cout << req.method << " " << req.url << " " << req.version << endl;
    cout << "path" << req.path << endl;
    cout << "------------------ http request end   ----------------------" << endl;

    string respline = "HTTP/1.1 200 OK\r\n";
    string respblank = "\r\n";
    string body;
    if (!util::readFile(req.path, &body)) {
        util::readFile(not_found_path, &body);
    }

    string respheader = "Content-Length: ";
    respheader += std::to_string(body.size());
    respheader += "\r\n";
    respheader += "Set-Cookie: 123456\r\n";
    resp.outbuffer = respline + respheader + respblank + body;
    cout << "------------------ http response start ----------------------" << endl;
    cout << resp.outbuffer;
    cout << "------------------ http response start ----------------------" << endl;

    // HttpLib demo done!
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        Usage();
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]);

    std::unique_ptr<httpServer> server(new httpServer(port));

    server->initServer();
    server->start(callback);

    return 0;
}
