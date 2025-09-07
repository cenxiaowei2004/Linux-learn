calClient:calClient.cpp
	g++ calClient.cpp -o calClient -std=c++11 -ljsoncpp
calServer:calServer.cpp
	g++ calServer.cpp -o calServer -std=c++11 -ljsoncpp
# -ljsoncpp 链接 jsoncpp 库
# -D MySelf 自定义 MySelf 宏
.PHONY:all
all:calClient calServer

.PHONY:clean
clean:
	rm -f calClient
	rm -f calServer
	