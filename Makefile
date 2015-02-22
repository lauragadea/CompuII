CC = gcc

CFLAGS = -g -Wall -pthread -fno-stack-protector
TARGETS = twittie

all: $(TARGETS)

twittie: main.c twitter.c hijo.c padre.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)


clean:
	rm -fv
	
