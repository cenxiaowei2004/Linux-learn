.PHONY:all
all:server cilent

server:shm_server.cpp
	g++ -o $@ $^ -std=c++11 

cilent:shm_cilent.cpp
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -rf server cilent