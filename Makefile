CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDFLAGS)

test: main
	./test.sh

clean:
	rm -f *.o tmp*

.PHONY: test clean

