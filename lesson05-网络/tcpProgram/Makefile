tcpCilent:tcpClient.cpp
	g++ tcpClient.cpp -o tcpClient -std=c++11
tcpServer:tcpServer.cpp
	g++ tcpServer.cpp -o tcpServer -std=c++11

.PHONY:all
all:tcpClient tcpServer

.PHONY:clean
clean:
	rm -f tcpClient
	rm -f tcpServer