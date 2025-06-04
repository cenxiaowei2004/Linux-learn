mylib=libcal.a
CC=gcc

$(mylib):add.o
	ar -rc -o $(mylib) $^
%.o:%.c
	$(CC) -fPIC -c $<


.PHONY:	clean
clean:
	rm -f $(mylib) ./*.o

.PHONY:output
output:
	mkdir -p mathlib/include
	mkdir -p mathlib/lib
	cp ./*.h mathlib/include
	cp ./*.so mathlib/lib

