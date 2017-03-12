

CFLAGS+= -g -ffunction-sections -fdata-sections

all : nanotest.o
clean:
	$(RM) *.o example_test
check: example_test
	./example_test

nanotest.o : nanotest.c nanotest.h

example_test.o : example_test.c nanotest.h
example_test : example_test.o nanotest.o
	$(CC) -g -o $@ example_test.o nanotest.o

