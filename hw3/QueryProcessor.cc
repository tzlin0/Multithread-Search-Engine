/*
 * Copyright ©2022 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2022 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace hw3 {

// the help_result struct is similar to the QueryResult class, but instead of
// storing doc_name, it stors doc_id, which is useful for doc lookup
struct help_result {
  DocID_t docid;
  int32_t rank;
};

// searches one idx at a time, go through all
// the query and store the help_result
// vector in the pointer, then return
void OneIdx(FILE* f, string idx, vector<help_result>
            * const mid_result, const vector<string>& query) {
  // convert hlr to final_result
  FileIndexReader* FIR = new FileIndexReader(idx);
  // the starting byte of index
  IndexFileHeader IDHD = FIR->getHeader();
  delete FIR;
  // go to IndexTableReader
  IndexTableReader *itr = new IndexTableReader(f,
  static_cast<uint>(sizeof(IndexFileHeader) + IDHD.doctable_bytes));
  // lookup the first word
  DocIDTableReader* ditr = itr->LookupWord(query[0]);
  if (ditr == static_cast<DocIDTableReader*>(nullptr)) {  // word not found
    delete ditr;
    delete itr;
    return;
  } else {  // word found in index
    list<DocIDElementHeader> hdlst = ditr->GetDocIDList();
    for (auto iter = hdlst.begin(); iter != hdlst.end(); iter++) {
      // declare a new help_result struct and append to mid_result;
      help_result * hr = new help_result;
      hr->docid = iter->doc_id;
      hr->rank = iter->num_positions;
      mid_result->push_back(*hr);
      delete hr;
    }
    // delete dtr;
  }
  int qs = query.size();
  if (qs != 1) {
    for (int i = 1; i < qs; i++) {
      // look up the next query word in index. If there are no matches,
      // it means the overall query should return an empty vector
      ditr = itr->LookupWord(query[i]);
      if (ditr == static_cast<DocIDTableReader*>(nullptr)) {
        // clear the vector then return
        mid_result->clear();
        delete ditr;
        delete itr;
        return;
      }

      // interesting part, iterate through all doc_ids in vector
      DocID_t doc_id;
      help_result hlr;
      // create a matchlist that loads the offsets from LookupDocID
      list<DocPositionOffset_t> match_list;
      // int num_docs = mid_result->size();
      for (auto it = mid_result->begin(); it != mid_result->end(); ) {
        hlr = *it;
        doc_id = hlr.docid;
        if (ditr->LookupDocID(doc_id, &match_list) == true) {
          // word found in DocIDTable
          // update the rank and load the hlr back to vector
          hlr.rank += match_list.size();
          *it = hlr;
          // and clear the match_list
          match_list.clear();
          it++;
        } else {
          mid_result->erase(it);
          match_list.clear();
        }
      }
    }
  }
}
QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int     rank;    // The rank of the result so far.
} IdxQueryResult;

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;
  // the first index (.idx)
  for (auto idx_it = index_list_.begin();
       idx_it != index_list_.end(); idx_it++) {
    vector<help_result> hlr;
    // open a file, but first set a new var* for the filename
    const string s = *idx_it;
    const char * ch = const_cast<const char *>(s.c_str());
    FILE* f = fopen(ch, "rb");
    Verify333(static_cast<FILE*>(nullptr) != f);
    DocTableReader *dtr = new DocTableReader(f, sizeof(IndexFileHeader));
    // go to OneIdx
    OneIdx(f, *idx_it, &hlr, query);
    // look up all the doc_id and update final_result
    string str;
    // loop through mid_result and append to final_result
    for (auto it = hlr.begin(); it != hlr.end(); it++) {
      Verify333(dtr->LookupDocID(it->docid, &str) == true);
      // declare a new QueryResult struct
      QueryResult *qr = new QueryResult;
      qr->document_name = str;
      qr->rank = it->rank;
      final_result.push_back(*qr);
      delete qr;
    }
  }
  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

}  // namespace hw3