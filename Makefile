CC = gcc
OBJS = parse.tab.o lex.yy.o funcs.o
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) `pkg-config --cflags glib-2.0`
LFLAGS = -Wall -lm $(DEBUG) `pkg-config --libs glib-2.0`

all: parser

parser: $(OBJS)
	@$(CC) $(LFLAGS) $(OBJS) -o parser

funcs.o: funcs.c funcs.h
	@$(CC) $(CFLAGS) funcs.c

lex.yy.o: token.l parse.tab.h funcs.h
	@flex token.l
	@$(CC) $(CFLAGS) lex.yy.c

parse.tab.h: parse.y
	@bison -d parse.y

parse.tab.c: parse.y
	@bison -d parse.y

parse.tab.o: parse.tab.h parse.tab.c funcs.h
	@$(CC) $(CFLAGS) parse.tab.c

clean:
	@rm *.o parser parse.tab.c lex.yy.c parse.tab.h
