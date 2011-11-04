CC = gcc
OBJECTS = parse.tab.o lex.yy.o funcs.o
DEBUG = -g
CFLAGS = -Wall $(DEBUG) `pkg-config --cflags glib-2.0`
LFLAGS = -Wall -lm $(DEBUG) `pkg-config --libs glib-2.0`

.PHONY: all
all: parser

parser: $(OBJECTS)
	@$(CC) $(LFLAGS) $(OBJECTS) -o parser

lex.yy.o: token.l parse.tab.h funcs.h
	@flex token.l
	@$(CC) $(CFLAGS) -c lex.yy.c

parse.tab.h: parse.y
	@bison -d parse.y

parse.tab.c: parse.y
	@bison -d parse.y

parse.tab.o: parse.tab.h parse.tab.c funcs.h
	@$(CC) $(CFLAGS) -c parse.tab.c

.PHONY: clean
clean:
	@rm *.o parser parse.tab.c lex.yy.c parse.tab.h
