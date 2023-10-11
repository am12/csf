CC= gcc
CFLAGS= -Wall -Wextra --pedantic


all: csim

csim: main.o util.o
	$(CC) -o csim main.o util.o

main.o: main.cpp util.h
	$(CC) $(CFLAGS) -c main.cpp

util.o: util.cpp util.h
	$(CC) $(CFLAGS) -c util.cpp

clean:
	rm -f *.o depend.mak

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak