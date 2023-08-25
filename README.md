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

### Comments
Comments in ThunderBird are similar to C and many other languages.
Single line comments begin with ```//``` and multi-line comments are contained within ```/*``` and ```*/```
Here is an example:
```
define int func() {
  let int x = 0; // This is a single-line comment

  /* 
     This is a
     multi-line
     comment
  */

  return x;
}
```


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

entry int main() {
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

### Philosphy
ThunderBird is built as an exercise to better learn compiler development. However, there are still some guiding principles that influenced its design.
1. Simplicity is powerful
    - I come from a background in C, and the simplicity of the language is admirable. 
    - The simplicity of C allows for the construction of complex types, and this is what is powerful. 
    - Rather than being built into the langauge, it is necessary that the programmer build something specific to their case. This is extremely annoying. But it does mean that little is hidden from both the writer and reader of the code. 
      - How does that queue work? It is likely in lib/queue.c and you can go look at it. 
      - When your library fails, it is YOUR library, so you GET to debug it rather than trying to figure out what went wrong when using someone else's obscure code you'll never look at like in React.
2. Explicicity
    - This might be personal, but I find code that uses explicit type declarations to be nice and easy to read. This is likely from my C background
    - While the ```auto``` keyword in C++ is very nice and I use it loosely in this compiler, I prefer code that declares the type along with the variable.
    - This mainly comes from my experience in Go where the convention is type inference.
        - There is nothing wrong with this and is one of the nicer features of Go, but this requires you to declare you variable and initialize it at the same time. This means that variable declarations/initializations are going on throughout the entire codebase, and it is hard to organize them to be all decalred at once, which is my prefered style.
        - The other reason I want to prevent type inference like in Go is that I don't know what types my variables are in Go because I never have to figure them out to declare them. Then when I want to pass them into a function and I do have to declare a type there, I have to retroactively figure it out. This is not fun, and honestly kind of ruins the convenience of type inference since I have to spend the time figuring it out anyway. ThunderBird prevents this by enforcing all types to be declared at variable declaration with no type inference.
    - I promise I know how to implement type inference pls believe me pls
3. Fuck OOP but I fuck with member functions frfr
    - So when using C, I would prefer to use member functions, but I also don't want to go into full OOP with polymorphism and inheritance.
    - I think combining data structures with logic is a natural way to think, and the syntax of ```mystring.append("this")``` is nicer in my opinion that ```string_append(mystring, "this");```
    - There is no ```private``` or ```public``` like in C++ despite having member functions.
        - This is because I like the concept in C of all data being public.
        - You CAN use member functions, or you could just do C-style programming like God intended. But for grug brains like me I like the ```.``` operator and it tingles my brain

## Potential Features in the Future
* Type casting
     * ```let int x = 2 + 3.0;``` -- the ```3.0``` should be cast from ```float``` to ```int```. As of now, this is not supported and will flag an error
* Strings
    * strings will be implimented in the standard library as a class rather than an inherent data structure
