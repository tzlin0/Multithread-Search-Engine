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

#include <stdio.h>       // for snprintf()
#include <charconv>      // char conversion
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.


#include "./ServerSocket.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:
  // Populate the "hints" addrinfo structure for getaddrinfo().
  // ("man addrinfo")
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;       // IPv6 (also handles IPv4 clients)
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
  hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // Use argv[1] as the string representation of our portnumber to
  // pass in to getaddrinfo().  getaddrinfo() returns a list of
  // address structures via the output parameter "result".
  char* port_ch = new char[5];
  std::to_chars(port_ch, port_ch + 5, port_);
  struct addrinfo* result;
  int res = getaddrinfo(nullptr, port_ch, &hints, &result);

  // Did addrinfo() fail?
  if (res != 0) {
    std::cerr << "getaddrinfo() failed: ";
    std::cerr << gai_strerror(res) << std::endl;
    return false;
  }

  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  int lfd = -1;
  for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
    lfd = socket(rp->ai_family,
                       rp->ai_socktype,
                       rp->ai_protocol);
    if (lfd == -1) {
      // Creating this socket failed.  So, loop to the next returned
      // result and try again.
      std::cerr << "socket() failed: " << strerror(errno) << std::endl;
      lfd = 0;
      continue;
    }

    // Configure the socket; we're setting a socket "option."  In
    // particular, we set "SO_REUSEADDR", which tells the TCP stack
    // so make the port we bind to available again as soon as we
    // exit, rather than waiting for a few tens of seconds to recycle it.
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof(optval));

    // Try binding the socket to the address and port number returned
    // by getaddrinfo().
    if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      // Bind worked!  Print out the information about what
      // we bound to.
      sock_family_ = rp->ai_family;
      break;
    }

    // The bind failed.  Close the socket, then loop back around and
    // try the next address/port returned by getaddrinfo().
    close(lfd);
    lfd = -1;
  }
  listen_sock_fd_ = lfd;
  *listen_fd = listen_sock_fd_;

  // Free the structure returned by getaddrinfo().
  freeaddrinfo(result);

  // Did we succeed in binding to any addresses?
  if (listen_sock_fd_ == -1) {
    // No.  Quit with failure.
    std::cerr << "Couldn't bind to any addresses." << std::endl;
    return false;
  }


  // Listen
  // Success. Tell the OS that we want this to be a listening socket.
  if (listen(listen_sock_fd_, SOMAXCONN) != 0) {
    std::cerr << "Failed to mark socket as listening: ";
    std::cerr << strerror(errno) << std::endl;
    close(listen_sock_fd_);
    return false;
  }

//   // Sleep for 20 seconds, then close house.
//   sleep(20);
//   close(lfd);
//   return EXIT_SUCCESS;

  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:
  // ?? block until a new connection
  struct sockaddr_storage caddr;
  socklen_t caddr_len = sizeof(caddr);
  *accepted_fd = accept(listen_sock_fd_, reinterpret_cast
                        <struct sockaddr*>(&caddr), &caddr_len);
  if (*accepted_fd < 0 && !((errno == EINTR) || (errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))) {
    std::cerr << "Failure on accept: " << strerror(errno) << std::endl;
    return false;
  }

  // return the params
  // client_addr
  struct sockaddr* c_saddr = reinterpret_cast<struct sockaddr*>(&caddr);
  if (c_saddr->sa_family == AF_INET) {
    // Print out the IPV4 address and port

    char astring[INET_ADDRSTRLEN];
    struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(c_saddr);
    inet_ntop(AF_INET, &(in4->sin_addr), astring, INET_ADDRSTRLEN);
    *client_addr = std::string(astring);  // cast char* to std::string
    *client_port = ntohs(in4->sin_port);  // return port value
    std::cout << " IPv4 address " << astring;
    std::cout << " and port " << ntohs(in4->sin_port) << std::endl;

  } else if (c_saddr->sa_family == AF_INET6) {
    // Print out the IPV6 address and port

    char astring[INET6_ADDRSTRLEN];
    struct sockaddr_in6* in6 = reinterpret_cast<struct sockaddr_in6*>(c_saddr);
    inet_ntop(AF_INET6, &(in6->sin6_addr), astring, INET6_ADDRSTRLEN);
    *client_addr = std::string(astring);  // cast char* to std::string
    *client_port = ntohs(in6->sin6_port);  // return port value
    std::cout << " IPv6 address " << astring;
    std::cout << " and port " << ntohs(in6->sin6_port) << std::endl;

  } else {
    std::cerr << " Address and port failed..." << std::endl;
    return false;
  }

  char hostname[1024];
  if (getnameinfo(c_saddr, caddr_len, hostname, 1024, nullptr, 0, 0) != 0) {
      std::cerr << "DNS failed" << std::endl;
  }
  *client_dns_name = std::string(hostname);

  char hname[1024];
  hname[0] = '\0';
  if (sock_family_ == AF_INET) {
    // The server is using an IPv4 address.
    struct sockaddr_in srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET_ADDRSTRLEN];
    getsockname(*accepted_fd,
                reinterpret_cast<struct sockaddr*>(&srvr),
                &srvrlen);

    inet_ntop(AF_INET, &srvr.sin_addr, addrbuf, INET_ADDRSTRLEN);
    *server_addr = std::string(addrbuf);

    getnameinfo(reinterpret_cast<struct sockaddr*>(&srvr),
                srvrlen, hname, 1024, nullptr, 0, 0);
    *server_dns_name = std::string(hname);
  } else {
    // The server is using an IPv6 address.
    struct sockaddr_in6 srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET6_ADDRSTRLEN];

    getsockname(*accepted_fd,
                reinterpret_cast<struct sockaddr*>(&srvr),
                &srvrlen);

    inet_ntop(AF_INET6, &srvr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
    *server_addr = std::string(addrbuf);

    getnameinfo(reinterpret_cast<struct sockaddr*>(&srvr),
                srvrlen, hname, 1024, nullptr, 0, 0);
    *server_dns_name = std::string(hname);
  }

  return true;
}

}  // namespace hw4
