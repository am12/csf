#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::cerr;
using std::string;

int main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  string username = argv[3];
  string room_name = argv[4];

  Connection conn;

  // TODO: connect to server

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)


  return 0;
}
