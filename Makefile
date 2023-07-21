CC=g++
CFLAGS=-g -Wall -Isrc/lib
EXECS=bin/thunder
LIB=lib/lexer.a lib/parser.a lib/ast.a

all: $(EXECS)

clean:
	rm -f bin/* obj/* lib/*

run: $(EXECS)
	./bin/thunder


$(EXECS): src/thunderbird.cc $(LIB)
	$(CC) $(CFLAGS) $< -o $@ $(LIB)

lib/lexer.a: obj/lexer.o
	ar ru $@ $<
	ranlib $@

obj/lexer.o: src/lib/lexer.cc
	$(CC) $(CFLAGS) -c $< -o $@

lib/ast.a: obj/ast.o
	ar ru $@ $<
	ranlib $@

obj/ast.o: src/lib/ast.cc
	$(CC) $(CFLAGS) -c $< -o $@

lib/parser.a: obj/parser.o
	ar ru $@ $<
	ranlib $@

obj/parser.o: src/lib/parser.cc
	$(CC) $(CFLAGS) -c $< -o $@
