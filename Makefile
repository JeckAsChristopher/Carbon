CC = gcc
CFLAGS = -Wall

all: carbon

carbon: main.o lexer.o runtime.o error.o parser.o
	$(CC) $(CFLAGS) -o carbon main.o lexer.o runtime.o error.o parser.o

main.o: main.c lexer.h runtime.h parser.h error.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

runtime.o: runtime.c runtime.h
	$(CC) $(CFLAGS) -c runtime.c

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

parser.o: parser.c parser.h lexer.h runtime.h error.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f *.o carbon
