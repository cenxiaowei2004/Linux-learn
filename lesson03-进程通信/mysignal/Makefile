# .PHONY:all
# all:test Mysignal

# test:test.cpp
# 	g++ -o $@ $^ -std=c++11 -g

# Mysignal:mysignal.cpp
# 	g++ -o $@ $^ -std=c++11

# .PHONY:clean
# clean:
# 	rm -rf test Mysignal


.PHONY:all
all: Mysignal

# test:test.cpp
# 	g++ -o $@ $^ -std=c++11 -g

Mysignal:mysignal.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf Mysignal