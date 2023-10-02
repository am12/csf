CC = gcc
CFLAGS = -Og -no-pie -Wall -Wextra -fno-stack-protector

all : danger danger.s

danger : danger.c
	$(CC) $(CFLAGS) -o $@ danger.c

# Use this target to generate assembly code for danger.c
danger.s : danger.c
	$(CC) $(CFLAGS) -S danger.c

clean :
	rm -f danger danger.s
