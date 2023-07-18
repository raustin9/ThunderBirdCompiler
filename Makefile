CC=g++
CFLAGS=-g -Wall -Isrc/lib
EXECS=bin/thunderbird
LIB=lib/lexer.a

all: $(EXECS)

clean:
	rm -f bin/* obj/* lib/*

run: $(EXECS)
	./bin/thunderbird


$(EXECS): src/thunderbird.cpp $(LIB)
	$(CC) $(CFLAGS) $< -o $@ $(LIB)

lib/lexer.a: obj/lexer.o
	ar ru $@ $<
	ranlib $@

obj/lexer.o: src/lib/lexer.cc
	$(CC) $(CFLAGS) -c $< -o $@
