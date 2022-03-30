# Bug 1

## A) How is your program acting differently than you expect it to?
- in our linkedlist method, there are memory leaks when we are 
- running valgrind saying that we have indirect losses. 

## B) Brainstorm a few possible causes of the bug
- Recalling TA session, there was a case that we have to free the 
- member pointers in the struct and then free the struct itself.
- 

## C) How you fixed the bug and why the fix was necessary
- We ended up finding that we freed the linkedlist without freeing 
the linkedlist tail, so we freed the tail, and then free the linked list, which eventually leads to passing the valgrind test.


# Bug 2

## A) How is your program acting differently than you expect it to?
- in our hashtable.c step 1, there is a case where we want to access the node in the linkedlist, but the system says that the member cannot be called, returned error.

## B) Brainstorm a few possible causes of the bug
- there is an include error 
- an incorrect type call/ struct
- System does not successfully read all header file

## C) How you fixed the bug and why the fix was necessary
- Eventually, we figured out that the node head and tail are declared only in LinkedList_priv , so that we cannot call those attributes. Instead, we have to use the given LLiterate functions to access the node and payload details


# Bug 3

## A) How is your program acting differently than you expect it to?
- During the ./test_suite testing, we found out that there is no returning key and value in the *oldkeyvalue pointer, which further caused a segments in fault.

## B) Brainstorm a few possible causes of the bug
- the helper function didn’t take in the key and *oldkeyvalue
- the while loop not having the right conditions
- the void pointer casting problem

## C) How you fixed the bug and why the fix was necessary
- After some time in gdb, we found that in chain_Lookup, once curr is declared, it has 0 for both key and value, and since our target key is also 0, it accidentally meets the while loop condition, so it returns a false key and an invalid value. So we changed the while loop and make it not include the curr->key == key condition.
