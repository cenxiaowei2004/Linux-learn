udpCilent:udpClient.cpp
	g++ udpClient.cpp -o udpClient -std=c++11
udpServer:udpServer.cpp
	g++ udpServer.cpp -o udpServer -std=c++11

.PHONY:all
all:udpClient udpServer

.PHONY:clean
clean:
	rm -f udpClient
	rm -f udpServer