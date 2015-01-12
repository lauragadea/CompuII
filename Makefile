CC = gcc

CFLAGS = -g -Wall
TARGETS = twittie

all: $(TARGETS)

twittie: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)


clean:
	rm -fv 
	
