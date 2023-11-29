#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::cerr;
using std::cin;
using std::string;
using std::stoi;
using std::getline;

int main(int argc, char **argv) {
  if (argc != 4) {
    cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  string server_hostname;
  int server_port;
  string username;

  server_hostname = argv[1];
  server_port = stoi(argv[2]);
  username = argv[3];

  // connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);

  // check if connection was successful
  if (!conn.is_open()) {
    cerr << "Sender connection failed\n";
    conn.close();
    return 1;
  }

  // send slogin message
  Message slogin_msg = Message(TAG_SLOGIN, username);
  if (!conn.send(slogin_msg)) {
    cerr << slogin_msg.data;
    conn.close();
    return 1;
  }

  // wait for response
  Message slogin_response;
  if (!conn.receive(slogin_response) || slogin_response.tag == TAG_ERR) {
    cerr << slogin_response.data;
    conn.close();
    return 1;
  }

  // loop reading commands from user, sending messages to server as appropriate
  while (1) {
    Message msg;
    string command;
    getline(cin, command);

    if (command.empty()) {
      continue;
      
    } else if (command == "/quit") {
      msg.tag = TAG_QUIT;
      msg.data = "bye";
      if (!conn.send(msg)) {
        cerr << msg.data;
        conn.close();
        return 1;
      } 
      Message quit_response;
      if (!conn.receive(quit_response) || quit_response.tag == TAG_ERR || 
           conn.get_last_result() == Connection::INVALID_MSG) {
        cerr << quit_response.data;
        conn.close();
        return 1;
      }
      break;

    } else if (command.substr(0, 5) == "/join") {
      msg.tag = TAG_JOIN;
      msg.data = command.substr(6);

    } else if (command == "/leave") {
      msg.tag = TAG_LEAVE;

    } else {
      msg.tag = TAG_SENDALL;
      msg.data = command;
    }

    if (!conn.send(msg)) {
      cerr << "Failed to send message: " << msg.data;
      conn.close();
      return 1;
    }
    Message response;
    if (!conn.receive(response) || response.tag == TAG_ERR ||
         conn.get_last_result() == Connection::INVALID_MSG) {
      cerr << response.data;
      conn.close();
      return 1;
    }
  }

  conn.close();
  return 0;
}