+-------------------------+
|  Bellamkonda Kartheek   |
|  190101023              |
+-------------------------+

Environment:
g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

Installation:
sudo apt-get install flex bison

command to run the program:
    make clean
    make
    ./main filename eg: "./main incorrect.txt"

If no file is specified it will take incorrect file as default input. 

Files Description:

incorrect.txt   :     It contains the input file to the Program with some errors.
correct.txt     :     It contains the input file to the Program without any errors.
lex.l           :     Lexical analyser.
main.cpp        :     Main function.
yacc.y          :     BNF rules are been defined here and it will take care of the syntax and semantic errors.
Makefile        :     To run all the files.

Handled Errors:

Syntax Errors   :   Variables defined with a reserved keyword,
                    statements that are not following the grammar,
                    foreign characters.

Semantic Errors :   Duplicate declaration of variables,
                    Variables defined but not used,
                    Conflicting variable types in expressions and assignments.

Output:

If any errors are there then the errors would be printed otherwise a msg indiacting no errors will be printed.