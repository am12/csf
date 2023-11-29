#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::cerr;
using std::cout;
using std::stoi;
using std::string;
using std::vector;
using std::getline;
using std::istringstream;

int main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  string server_hostname = argv[1];
  int server_port = stoi(argv[2]);
  string username = argv[3];
  string room_name = argv[4];

  // connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);

  // check if connection was successful
  if (!conn.is_open()) {
    cerr << "Receiver connection failed\n";
    return 1;
  }

  // send rlogin message
  conn.send(Message(TAG_RLOGIN, username));
  
  Message rlogin_response;
  conn.receive(rlogin_response);

  if (rlogin_response.tag == TAG_ERR) {
    cerr << rlogin_response.data;
    conn.close();
    return 1;
  }

  // send join room message
  conn.send(Message(TAG_JOIN, room_name));

  Message join_response;
  conn.receive(join_response);
  
  if (join_response.tag == TAG_ERR) {
    cerr << join_response.data;
    conn.close();
    return 1;
  }
  
  // loop waiting for messages from server (which should be tagged with TAG_DELIVERY)
  while (1) {
    Message server_msg;
    conn.receive(server_msg);

    if (server_msg.tag == TAG_DELIVERY) {
      string s = server_msg.data;
      istringstream ss(s);
      string token;

      getline(ss, token, ':'); // ignore
      getline(ss, token, ':');
      string data1 = token;

      getline(ss, token, ':');
      string data2 = token;

      cout << data1 << ": " << data2;
    }
  }

  conn.close();
  return 0;
}
