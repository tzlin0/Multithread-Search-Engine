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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;

#define BUFSIZE 256

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:

  // create a vector to contain the str
  vector<string> requests;
  unsigned char buf[BUFSIZE];
  // create 2 strings
  string first;
  int count = 0;

  auto index = buffer_.find(string(kHeaderEnd));
    while (string::npos == index && count <= 3) {
    int res = WrappedRead(fd_, buf, BUFSIZE);
    if (res < 0) {
      cout << "WrappedRead fail" << endl;
      return false;
    } else if (res ==  0) {
      cout << "WrappedRead empty" << endl;
      return false;
    }
    size_t buf_len = buffer_.length();
    buffer_ += string(reinterpret_cast<char *>(buf));
    buffer_ = buffer_.substr(0, buf_len + res);
    index = buffer_.find(string(kHeaderEnd));
    count++;
  }
  first = buffer_.substr(0, index);
  buffer_ = buffer_.substr(index + 4);

  *request = ParseRequest(first);
  return true;
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:
  string copy(request);
  boost::algorithm::trim(copy);
  boost::algorithm::to_lower(copy);

  vector<string> lines;
  boost::split(lines, copy, boost::is_any_of("\r\n"), boost::token_compress_on);
  vector<string> components;

  // get uri from the first line and set it
  string firstline = lines[0];
  boost::split(components, firstline, boost::is_any_of(" "),
               boost::token_compress_on);
  req.set_uri(components[1]);

  // the rest of the lines
  for (auto line = lines.begin()+1; line != lines.end(); line++) {
    vector<string> nnv;
    boost::split(nnv, *line, boost::is_any_of(": "), boost::token_compress_on);
    req.AddHeader(nnv[0], nnv[1]);
  }
  // cout << "finish parsing" << endl;




  return req;
}

}  // namespace hw4
