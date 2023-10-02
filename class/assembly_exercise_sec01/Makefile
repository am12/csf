CC = gcc
CFLAGS = -g -Wall -std=gnu11 -no-pie
ASMFLAGS = -g -no-pie
LDFLAGS = -no-pie

C_SRCS = paltests.c c_is_palindrome.c
ASM_SRCS = asm_is_palindrome.S

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.o : %.S
	$(CC) $(ASMFLAGS) -c $*.S -o $*.o

all : c_paltests asm_paltests

c_paltests : paltests.o c_is_palindrome.o tctest.o
	$(CC) $(LDFLAGS) -o $@ paltests.o c_is_palindrome.o tctest.o

asm_paltests : paltests.o asm_is_palindrome.o tctest.o
	$(CC) $(LDFLAGS) -o $@ paltests.o asm_is_palindrome.o tctest.o

clean :
	rm -f c_paltests asm_paltests *.o depend.mak

depend :
	$(CC) $(CFLAGS) -M $(C_SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
