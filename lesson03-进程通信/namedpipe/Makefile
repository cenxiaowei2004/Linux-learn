.PHONY:all
all:myserver myclient

myserver:server.cpp
	g++ -o $@ $^ -std=c++11 -g

myclient:client.cpp
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm -f myserver myclient