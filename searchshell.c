/*
 * Copyright 2022 Cheng-Hu Lin Modified from the CSE333 class from University of
 * Washington, the Department of Computer-Science
 */

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc
static void Usage(void);

// splits the input string into char** array, and does the search, prints
// out the result
static void ProcessQueries(DocTable* dt, MemIndex* mi, char*mainStr);
// static int GetNextLine(FILE* f, char** ret_str);

//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char** argv) {
  if (argc != 2) {
    Usage();
  }
  DocTable* dt;
  MemIndex* idx;
  int res;

  size_t bufsize = 100;
  char buffer[100];
  char *input = buffer;

  // Crawl
  printf("Indexing '%s'\n",argv[1]);
  res = CrawlFileTree(argv[1], &dt, &idx);

  Verify333(res == 1);  // assume that it's crawled
  // scan input
  printf("enter query:\n");
  // while (getline(&input, &bufsize, stdin) != -1) {
  //   // processQueries
  //   ProcessQueries(dt, idx, input);
  //   // print the next prompt
  //   printf("enter query:\n");
  // }
  while (fgets(buffer, 100, stdin)) {
    // strcat(text, buffer);
    // processQueries
    // buffer[3] = "\0";
    ProcessQueries(dt, idx, buffer);
    // print the next prompt
    printf("enter query:\n");
  }
  // while EOF is not scanned
  // printf("enter query:\n");
  // while (getline(&input, &bufsize, stdin) != -1) {
  //   // processQueries
  //   ProcessQueries(dt, idx, input);
  //   // print the next prompt
  //   printf("enter query:\n");
  // }

  // while (scanf("%99s", input) != EOF) {
  //   // processQueries
  //   // input = {"man","manual"};
  //   ProcessQueries(dt, idx, input);
  //   // print the next prompt
  //   printf("enter query:\n");
  // }
  //
  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.
  MemIndex_Free(idx);
  DocTable_Free(dt);
  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(DocTable* dt, MemIndex* mi, char*mainStr) {
  // read the str
  char delim[] = " ";
  int num = 0;
  // copy the original str to count the words first
  char * firststr = (char*) malloc(strlen(mainStr) + 1);
  char * newstr = (char*) malloc(strlen(mainStr) + 1);
  snprintf(firststr, strlen(mainStr)+1, mainStr);
  snprintf(newstr, strlen(mainStr)+1, mainStr);
  // set the length of the char[];

  // char *np = strtok(newstr, delim);
  // while (np != NULL) {
  //   num++;
  //   np = strtok(NULL, delim);
  // }
  char *token;
  firststr[strlen(firststr)-1] = '\0';

  char *rest = firststr;

  while ((token = strtok_r(newstr, " ", &newstr))) {
    num++;
  }
  // free(newstr);
  // set the query[]
  char *query[num];
  // char *ptr = strtok(firststr, delim);
  int i = 0;


  // while((token = strtok_r(rest, " ", &rest))) {
  //   query[i++] = token;
  // }
  while (1) {
        token = strtok_r(firststr, " ", &firststr);
        if (token == NULL)
          break;
        query[i] = token;
        i++;
        rest = NULL;
}
// free(firststr);
  // query created
  ////////////////////
  // user input
  // query[2] = {"man", "manual"};

  // over write

  // output process
  LinkedList* ll_res = MemIndex_Search(mi, query, num);  // 2 for two items
  if (ll_res == NULL) {
    printf("No data, please try again!\n");
    return;
  }
  int n = LinkedList_NumElements(ll_res);
  LLIterator* lit = LLIterator_Allocate(ll_res);

  // print out result
  for (int i = 0; i < n; i++) {
    SearchResult* res;

    LLIterator_Get(lit, (LLPayload_t *) &res);
    char* doc_name = DocTable_GetDocName(dt, res->doc_id);
    printf("  %s (%d)\n", doc_name, res->rank);
    // next one
    LLIterator_Next(lit);
  }
  LLIterator_Free(lit);
  LinkedList_Free(ll_res, (LLPayloadFreeFnPtr)free);
  // free(firststr);
  // free(newstr);
}
