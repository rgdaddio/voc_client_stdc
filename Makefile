src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc -g -Wall -std=c99
#LDFLAGS = -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_regex -lcrypto -lssl -ljson-c -lsqlite3
LDFLAGS = -ljson-c -lsqlite3 -lssl -lcrypto

voc_client_app: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f voc_client_app *~ *.o
