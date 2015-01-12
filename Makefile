CC = gcc

CFLAGS = -g -Wall
TARGETS = twittie

all: $(TARGETS)

twittie: main.c checkCommand.c childService.c getTimeline.c showTimeline.c verifyTweetLength.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)


clean:
	rm -fv 
	
