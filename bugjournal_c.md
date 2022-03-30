# Bug 1

## A) How is your program acting differently than you expect it to?
- The WordPosition struct's word field is returning a word that doesn't match the ones that
were added previously. In one instance, it is returning an empty string when we should be 
seeing the word article.

## B) Brainstorm a few possible causes of the bug
- Another part of the program uses the word for a different purpose and ends up
courrupting the word
- The word isn't being parsed correctly
- The word exists on the stack and is being used for other method calls once its reaped 

## C) How you fixed the bug and why the fix was necessary
- Instead of storing the word parameter passed into the ParseIntoWordPositionsTable method,
we store a copy of the word onto the heap. This fix is necessary since the program will use
the word later on once the ParseIntoWordPositionsTable method completes. If we don't store it
on the heap, it's location on the stack will be replaced by other data. 

# Bug 2

## A) How is your program acting differently than you expect it to?
- In the Memindex.c file, the (SearchResult) is read correctly in step 6 

## B) Brainstorm a few possible causes of the bug
- ret_list is not constructed correctly in step 4
- ret list in not read correctly in step 6
- pointer address lost among the loops

## C) How you fixed the bug and why the fix was necessary
- Checked every srptr->doc_id in test_suite using gdb, then found that the readings were not correct, so I went back and forth to check the LinkedList header files, and found that i should give the address of the srptr instead of itself.


# Bug 3

## A) How is your program acting differently than you expect it to?
- When running valgrind, we get an error saying that we are not freeing up the entire
heap once the program terminates. There are about 100,000 bytes not being freed

## B) Brainstorm a few possible causes of the bug
- Creating more structures then needed 
- Not freeing structures
- Free statements with incorrect parameters

## C) How you fixed the bug and why the fix was necessary
- The issue was with the words that were in the WordPositions structure. Once we passed
the ownership of the word stored in the FileParser HashTable to the MemIndex, we just used
the word to make a copy to put in the MemIndex. That's where we lost track of it.
So we just stored the word directly into the MemIndex from the FileParser HashTable.
This was neccisary becuase once we used the word to make a copy, we didn't free
it which led to the heap having data in it.
