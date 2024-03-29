CC=g++
CFLAGS=-g -Wall -Isrc/lib
EXECS=bin/thunder
LIB=lib/compiler.a lib/preprocessor.a lib/ast.a lib/errorhandler.a lib/symboltable.a lib/lexer.a lib/parser.a  

all: $(EXECS)

clean:
	rm -f bin/* obj/* lib/*

run: $(EXECS)
	./bin/thunder


$(EXECS): src/thunderbird.cc $(LIB)
	$(CC) $(CFLAGS) $< -o $@ $(LIB)

lib/compiler.a: obj/compiler.o
	ar ru $@ $<
	ranlib $@

obj/compiler.o: src/lib/compiler.cc
	$(CC) $(CFLAGS) -c $< -o $@

lib/errorhandler.a: obj/errorhandler.o
	ar ru $@ $<
	ranlib $@

obj/errorhandler.o: src/lib/errorhandler.cc
	$(CC) $(CFLAGS) -c $< -o $@

lib/symboltable.a: obj/symboltable.o
	ar ru $@ $<
	ranlib $@

obj/symboltable.o: src/lib/symboltable.cc
	$(CC) $(CFLAGS) -c $< -o $@

# Preprocessor
lib/preprocessor.a: obj/preprocessor.o
	ar ru $@ $<
	ranlib $@

obj/preprocessor.o: src/lib/preprocessor.cc
	$(CC) $(CFLAGS) -c $< -o $@

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
