CC=g++
CFLAGS=-g -Wall -Isrc/lib
EXECS=bin/thunder
LIB=lib/lexer.a

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
