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
The goal is to introduce a simple type system of ```int```s, ```float```s and ```bytes```, but I may expand it to support basic strings as well

## Compiler
All components of the compiler are built on my own without the use of any generators.
The point of this is to learn, so I did not want to use flex/bison which I have used before.

The scanner  (at the moment) is a single pass lexical analyzer. It simply iterates through the input string one character at a time, and matches
tokens with the corresponding rulesets.
The parser is a recursive descent parser that follows the basic grammar of the language

## Language Features
### Top Level Statements
At the top or "global" level, you can either define a function or a global variable
An example of this is:
```
let int global_x = 0;

define int example_func() {
  let int x = 2 + global_x;
  return x;
}
```
In this case, the variable ```global_x``` is at the global scope
An example of an illegal top-level statement is 
```
let int global_x = 0;
x + 4;
```
This is illegal because expressions as statements are not supported at the global level.
The only thing you can do outside of a function is define functions and variables

### Function Definitions
To define a function, use the ```define``` keyword.
A function definition requires a specified return type as well
Here is an example of a basic function definition:
```
define float example(float arg1, float arg2) {
  return arg1 + arg2;
}
```
See, simple.

### Variable declarations
Use the ```let``` keyword to declare a variable.
Variable declarations are required to be initialized to a value.

Here is an example of a legal declaration:
```let float x = 0.0;```
Here is an example of an illegal declaration:
```let float x;```
This could seem annoying, but it is something that I find useful.

For example, in C, sometimes ```int```s are defaulted to 0, and sometimes they are not. It is not hard to find out as you just have to look at your compiler version and details, but it is still irritating to have to keep that in mind.
In ThunderBird, this is done so that you always know what your variables are initialized to.
Isn't that nice?

You can also initialize your variables to expressions
```
let float x = 3.0 - 1.0;
```



## Potential Features in the Future
* Type casting
     * ```let int x = 2 + 3.0;``` -- the ```3.0``` should be cast from ```float``` to ```int```. As of now, this is not supported and will flag an error
* Strings
