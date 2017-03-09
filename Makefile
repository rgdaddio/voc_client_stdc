src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = g++ -std=c++11 -g -Wall
LDFLAGS = -ljson-c -lsqlite3

voc_client_app: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f voc_client_app *~
