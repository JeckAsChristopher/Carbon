CC = gcc
CFLAGS = -Wall

# Default target
all: carbon

# Linking the final binary
carbon: main.o lexer.o runtime.o error.o parser.o
	$(CC) $(CFLAGS) -o carbon main.o lexer.o runtime.o error.o parser.o

# Compilation rules
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

# Clean rule
clean:
	rm -f *.o carbon
