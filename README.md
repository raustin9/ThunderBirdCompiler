# ThunderBird Compiler

## Intent
This is meant to be a proof of concept or small compiler meant to better learn
the mechanics of compiler development.

## Language
This is a small language that will not have many features
Here is a small layout of a sample program:
```
function add(int x, int y) {
  int rv = x + y;
}

function main() {
  float fval = 2.0 + 4.0;
  int add_value = add(x, y);
}
```
The goal is to introduce a simple type system of ```int```s and ```float```s, but I may expand it to support basic strings as well

## Compiler
All components of the compiler are built on my own without the use of any generators.
The point of this is to learn, so I did not want to use flex/bison which I have used before.

The scanner  (at the moment) is a single pass lexical analyzer. It simply iterates through the input string one character at a time, and matches
tokens with the corresponding rulesets.
The parser is a recursive descent parser that follows the basic grammar of the language

## Potential Features in the Future
* Type casting
     * let int x = 2 + 3.0; -- the 3.0 should be cast from ```float``` to ```int```. As of now, this is not supported and will flag an error
* Strings
