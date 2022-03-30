# Bug 1

## A) How is your program acting differently than you expect it to?
- In fileIndexReader.cc, my program is not reading the 
- file index, namely the magic number, etc.

## B) Brainstorm a few possible causes of the bug
- read() failed
- fail to convert the bytes into uint and int
- 

## C) How you fixed the bug and why the fix was necessary
- I found out that instead of extracting the four fileIndex(16 bytes) manually, we could atcually instantiate a IndexFileHeader struct, and then simply put the struct 
in the read() function, the rest comes as a package.


# Bug 2

## A) How is your program acting differently than you expect it to?
- In QueryProcessor.cc, I am getting the wrong results when attempting to check multiple queries

## B) Brainstorm a few possible causes of the bug
- helper struct not built correctly
- helper function not getting the correct input values
- query vector iterator not set correctly

## C) How you fixed the bug and why the fix was necessary
- when we are going through the second word, we set an iterator that check from the start of the helper_struct 
all the way to the end, but for the case where we delete when we don't find the docID in the docIDtable,
we don't need to add the iterator, since if we call 
vector::erase, the iterator moves on to the next element automatically.


# Bug 3

## A) How is your program acting differently than you expect it to?
- In filesearchshell.cc. The program is not defining hw3::QueryProcessor when feeding in the index_vector idx.

## B) Brainstorm a few possible causes of the bug
- the searchQuery parameter has a bug on the pointer or reference type
- the searchQuery parameter has missuse on const
- hw3::QueryProcessor cannot take in vector<string>& idx.

## C) How you fixed the bug and why the fix was necessary
- The index_vector is in fact not an index_vector, but an index list, in order for the hw3::QueryProcessor to be defined, we needed 
an index_list, not an index vector. The SearchQuery function should feed in a parameter of const list<string>& idx instead of const vector<string>& idx.
