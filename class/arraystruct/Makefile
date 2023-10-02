CC = gcc

CFLAGS = -g -no-pie -Wall -Wextra -pedantic -std=c99
ASMFLAGS = -g -no-pie
LDFLAGS = -no-pie

EXES = arrptr ptrarith1 ptrarith2 sumelts1 sumelts2 \
	sumeltsasm1 sumeltsasm2 structlayout structlayout2 \
	structlayout3 structlayout4

%.o : %.S
	$(CC) $(ASMFLAGS) -c $*.S -o $*.o

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

% : %.o
	$(CC) $(LDFLAGS) -o $* $*.o

all : $(EXES)

clean :
	rm -f *.o $(EXES)
