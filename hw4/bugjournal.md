# Bug 1

## A) How is your program acting differently than you expect it to?
- There is a compile bug that does not allow me to instantiate a new FileReader class object

## B) Brainstorm a few possible causes of the bug
- Wrong input parameters numbers
- Wrong input parameter types
- Other weird problem

## C) How you fixed the bug and why the fix was necessary
- It turned out that I did a "f = (base_dir, mypath);"  instead of "f(base_dir, mypath);" adding another "=". Which may cause program to replace my first parameter with my second parameter.



# Bug 2

## A) How is your program acting differently than you expect it to?
- In HttpServer.cc, I failed to assign the read documents to the ret.body, leading to a compile error.

## B) Brainstorm a few possible causes of the bug
- Wrong initialization of the ret class 
- Wrong data type to pass in.
- Cannot access private members

## C) How you fixed the bug and why the fix was necessary
- The solution is about the private field, if we try to assign the ret.body, we have to use ret.AppendToBody instead of ret.body_ = ..., since the AppendToBody() is open to public whereas the ret.body_ member is not.


# Bug 3

## A) How is your program acting differently than you expect it to?
- Cannot get the readfile text

## B) Brainstorm a few possible causes of the bug
- bug in the filereader file
- bug in the ispathtrue function
- 

## C) How you fixed the bug and why the fix was necessary
- ispathtrue does not work so good 
