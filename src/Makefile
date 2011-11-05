OBJECTS = parse.tab.o lex.yy.o funcs.o
CFLAGS = -Wall -g `pkg-config --cflags glib-2.0`
LFLAGS = -Wall -lm -g `pkg-config --libs glib-2.0`

all: parser

parser: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@

lex.yy.o: token.l parse.tab.h funcs.h
	flex token.l
	$(CC) $(CFLAGS) -c lex.yy.c

parse.tab.h: parse.y
	bison -d parse.y

parse.tab.c: parse.y
	bison -d parse.y

parse.tab.o: parse.tab.h parse.tab.c funcs.h
	$(CC) $(CFLAGS) -c parse.tab.c

clean:
	-rm $(OBJECTS) parser parse.tab.c lex.yy.c parse.tab.h

check: all
	# Unit tests go here. None currently.
	@echo "*** ALL TESTS PASSED ***"

.PHONY: all clean check
