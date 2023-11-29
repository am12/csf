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
    return 1;
  }

  // send slogin message
  conn.send(Message(TAG_SLOGIN, username));

  Message slogin_response;
  conn.receive(slogin_response);

  if (slogin_response.tag == TAG_ERR) {
    cerr << slogin_response.data;
    conn.close();
    return 1;
  }

  // loop reading commands from user, sending messages to server as appropriate
  while (1) {
    Message msg;
    string command;
    getline(cin, command);

    if (command == "/quit") {
      msg.tag = TAG_QUIT;

      conn.send(msg);
      Message quit_response;
      conn.receive(quit_response);
      if (quit_response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
        cerr << quit_response.data;
        conn.close();
        return 1;
      }
      conn.close();
      return 0;

    } else if (command.substr(0, 5) == "/join") {
      msg.tag = TAG_JOIN;
      msg.data = command.substr(6);

    } else if (command == "/leave") {
      msg.tag = TAG_LEAVE;

    } else {
      msg.tag = TAG_SENDALL;
      msg.data = command;
    }

    conn.send(msg);
    Message response;
    conn.receive(response);
    if (response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
      cerr << response.data;
      conn.close();
      return 1;
    }
  }

  conn.close();
  return 0;
}