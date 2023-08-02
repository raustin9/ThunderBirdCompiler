# ThunderBird Compiler

## Intent
This is meant to be a proof of concept or small compiler meant to better learn
the mechanics of compiler development.

## Language
This is a small language that will not have many features
Here is a small layout of a sample program:
```
define int add(int x, int y) {
  let int rv = x + y;
  return rv;
}

entry int main() {
  let float fval = 2.0 + 4.0;
  let int add_value = add(x, y);
}
```
The goal is to introduce a simple type system of ```int```s, ```float```s and ```bytes```, but I may expand it to support basic strings as well
Strings will likely be an array of ```byte```s similar to how it is done in c

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

### Program entry point
To declare the entry point to your program, you will use the ```entry``` keyword. This word functions exactly the same as 
```define``` or ```function``` except that it can only be used once per program. The reasoning behind this is obvious as you 
can only have one entry point per program.
Here is an example:
```
entry int main_func() {
  return 0;
}
```
The beauty of using the ```entry``` keyword is that you do not have to use a specific name for your entry point like ```int main()``` in C & C++.


### Running a program
**Currently unsupported and subject to change**
To run a program, you will use the ```thunder``` command. 
If you have the file ```test.tb```, you will run the command ```thunder test.tb```
This will create a binary file ```test``` that you can run as normal

Here is an example file I am going to call ```example.tb```
```
define int integer_add(int n1, int n2) {
  return n1 + n2;
}

entr int main() {
  let int x = 0;
  let int y = 0;

  x = integer_add(1, 2) - 4;

  if (x < 0) {
    y = 1;
  } else {
    y = 2;
  }

  return 0;
}
```
In this example file, we have a ```integer_add()``` function that takes two integers and adds them together.  
Our entry point in the ```main``` function then does some basic arithmetic using that function, and includes some control flow via ``if`` statements.  

To compile this program, we need to run ```thunder example.tb`` and then run the program using ```./example```

### Compiling a basic example program
Here is an example file

## Potential Features in the Future
* Type casting
     * ```let int x = 2 + 3.0;``` -- the ```3.0``` should be cast from ```float``` to ```int```. As of now, this is not supported and will flag an error
* Strings
