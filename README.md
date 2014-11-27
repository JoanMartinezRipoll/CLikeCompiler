CLikeCompiler
=============
A C#-like language compiler. The language deals with the string and int types, allows method and variable declarations and includes arithmetic operations for the int type and concatenation and comparison operations for the string type. While sentences and a write method (output) are also considered.  Once compiled, the code is too executed.

The compiler performs both lexical and semantic analysis. Semantic analysis includes:
* Several parameters can be declared, but parameters with the same name must belong to different scopes (main or a given method).
* The previous applies too for method declarations. 
* Number of arguments and their type is checked when calling a method.
* Method call return value type must be equal to the one provided at the method declaration.
* Operations and assignations are only allowed between same types
* Checks weather the operation is available for a given type
* More...

The core files of this project are interpreter.cc and compiler.g

## LANGUAGE EXAMPLES
Language examples can be found at the TestSets folder. The sub-folder Errors includes code with semantical and lexical errors that will cause the compiler to display error messages.

## INSALL AND EXECUTION NOTES
The compiler needs pccts to be installed

`sudo apt-get install pccts`

To add the pccts libraries:

`ln -s /usr/include/pccts/* /usr/local/include/`

Run `make` to compile the compiler.
Then, the compiler can be run:

`./compiler< TestSets/ts2`

Run `make compilerclean` to clean
