project: main.o message.o
	g++ main.o message.o -o project

main.o: main.cpp
	g++ -c main.cpp

message.o: message.cpp
	g++ -c message.cpp

.PHONY:clean all
clean:
	rm *.o project
all:main.o message.o
	echo 'all done'

