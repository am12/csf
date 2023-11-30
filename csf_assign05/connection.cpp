#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
#include <string.h>
#include "csapp.h"
#include "message.h"
#include "connection.h"

using std::cerr;
using std::string;
using std::to_string;
using std::istringstream;
using std::getline;

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // call rio_readinitb to initialize the rio_t object
  Rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // call open_clientfd to connect to the server
  const char* hostname_str = hostname.c_str();
  const char* port_str = to_string(port).c_str();
  m_fd = Open_clientfd(hostname_str, port_str);

  if (m_fd <= 0) { 
    m_last_result = EOF_OR_ERROR;
    cerr << "Error: " << m_last_result << std::endl;
    return;
  } 

  // call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
  m_last_result = SUCCESS;
}

Connection::~Connection() {
  // close the socket if it is open
  close();
}

bool Connection::is_open() const {
  // return true if the connection is open
  if (m_fd >= 0) {
    return true;
  }
  return false;
}

void Connection::close() {
  // close the connection if it is open
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::validSend(const Message &msg) {
  if (msg.tag == TAG_ERR || msg.tag == TAG_OK || msg.tag == TAG_EMPTY || msg.tag == TAG_JOIN ) {
    return true;
  } else if (msg.tag == TAG_DELIVERY || msg.tag == TAG_LEAVE || msg.tag == TAG_QUIT) {
    if (msg.data.find(":")) {
      return true;
    }
  }
  return false;
}

bool Connection::validReceive(const Message &msg) {
  if (msg.data.find(":")){
    return true;
  }
  return false;
}
 
bool Connection::send(const Message &msg) {
  // send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  string message_cpp = msg.tag + ":" + msg.data + "\n";
  const char* message = message_cpp.c_str();
  ssize_t result = Rio_writen(m_fd, message, strlen(message));

  // error handling
  if (result < 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  } 
  
  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char buffer[Message::MAX_LEN + 1];
  ssize_t result = Rio_readlineb(&m_fdbuf, buffer, Message::MAX_LEN);

  // error handling
  if (result < 0) { 
    m_last_result = INVALID_MSG;
    return false;
  }

  // get tag and data from buffer
  char* str;
  str = strtok(buffer, ":");
  msg.tag = str;
  str = strtok(NULL, "");
  msg.data = str;  

  // successful exit
  m_last_result = SUCCESS;
  return true;
}
