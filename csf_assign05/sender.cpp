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
using std::cout;
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
    cerr << "Sender connection failed";
    return 1;
  }

  // send slogin message
  conn.send(Message(TAG_SLOGIN, username));

  Message slogin_response = Message();
  conn.receive(slogin_response);

  if (slogin_response.tag == TAG_ERR) {
    cerr << slogin_response.data;
    conn.close();
    return 1;
  }

  // loop reading commands from user, sending messages to server as appropriate
  while (1) {

    Message message;
    string input;
    getline(cin, input);

    if (input.substr(0, 6) == "/join ") {
      message.tag = TAG_JOIN;
      message.data = input.substr(6);
    } else if (input == "/leave") {
      message.tag = TAG_LEAVE;
    } else if (input == "/quit") {
      message.tag = TAG_QUIT;
      conn.send(message);
      Message quit_response = Message();
      conn.receive(quit_response);
      if (quit_response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
        cerr << quit_response.data;
      }
      break;
    } else {
      message.tag = TAG_SENDALL;
      message.data = input;
    }
    conn.send(message);
    Message message_response = Message();
    conn.receive(message_response);
    if (message_response.tag == TAG_ERR || conn.get_last_result() == Connection::INVALID_MSG) {
      cerr << message_response.data;
      continue;
    }
  }

  conn.close();
  return 0;
}
