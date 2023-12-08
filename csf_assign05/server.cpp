#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

using std::cerr;
using std::string;
using std::to_string;

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

// Info: keeps track of server and connection information as a group
struct Info {
  Connection *conn;
  Server *server;

  Info(Connection *conn, Server *server) : conn(conn), server(server) { }

  ~Info() {
    // destroy connection when object is destroyed
    delete conn;
  }
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

/// SENDER AND RECEIVER IMPLEMENTATIONS (START)

void receiver_chat(Connection *conn, Server *server, User *user) {
  Message message;
  Room *room = nullptr;

  // did not receive message
  if (!conn->receive(message)) {
    if (conn->get_last_result() == Connection::INVALID_MSG || conn->get_last_result() == Connection::EOF_OR_ERROR) {
      conn->send(Message(TAG_ERR, "invalid message"));
    } else {
      conn->send(Message(TAG_ERR, "receive failed"));
    }
    delete user;
    return;
  }

  // not a join message
  if (message.tag != TAG_JOIN) {
    conn->send(Message(TAG_ERR, "expected a join message"));
    delete user;
    return;
  }

  // message too long
  if (message.data.length() >= Message::MAX_LEN) {
    conn->send(Message(TAG_ERR, "message is too long"));
    delete user;
    return;
  }

  // joined a room
  if (!room) {
    room = server->find_or_create_room(message.data);
    room->add_member(user);
    if (!conn->send(Message(TAG_OK, "joined room"))) {
      delete user;
      return;
    }
  }

  // unload message_queue to receiver
  while (1) {
    Message *new_msg = user->mqueue.dequeue();

    if (new_msg) {
      if (!conn->send(*new_msg)) {
        delete new_msg;
        break;
      }
      delete new_msg;
    }
  }

  // cleanup
  if (room) {
    room->remove_member(user);
  }
  delete user;
  delete conn;
}


void sender_chat(Connection *conn, Server *server, User *user) {
  Room *room = nullptr;

  while (1) {
    Message message;

    // did not receive a message
    if (!conn->receive(message)) {
      if (conn->get_last_result() == Connection::INVALID_MSG || conn->get_last_result() == Connection::EOF_OR_ERROR) {
        conn->send(Message(TAG_ERR, "invalid message"));
      } else {
        conn->send(Message(TAG_ERR, "receive failed"));
      }
      return;
    } 

    // quit message
    if (message.tag == TAG_QUIT) {
      conn->send(Message(TAG_OK, "goodbye."));
      return;
    }

    // error message
    if (message.tag == TAG_ERR) {
      cerr << message.data;
      return;
    }

    // message too long
    if (message.data.length() >= Message::MAX_LEN) {
      conn->send(Message(TAG_ERR, "message is too long"));
    }

    // if user has not joined a room, join a room
    else if (!room) {
      if (message.tag == TAG_JOIN) {
        room = server->find_or_create_room(message.data);
        room->add_member(user);
        if (!conn->send(Message(TAG_OK, "joined room"))) {
          return;
        }
      } else {
        conn->send(Message(TAG_ERR, "must join room before sending a message"));
      }
    }

    // joining a new room (/join [room])
    else if (message.tag == TAG_JOIN) {
      room->remove_member(user);
      room = server->find_or_create_room(message.data);
      room->add_member(user);
      if (!conn->send(Message(TAG_OK, "switched rooms"))) {
        return;
      }
    }

    // leaving room (/leave)
    else if (message.tag == TAG_LEAVE) {
      // de-register sender from room
      if (room) {
        room->remove_member(user);
        room = nullptr;
        if (!conn->send(Message(TAG_OK, "left room"))) {
          return;
        }
      } else {
        conn->send(Message(TAG_ERR, "you are not in a room"));
      }
    } 

    // broadcast message ([msg])
    else if (message.tag == TAG_SENDALL) { 
      room->broadcast_message(user->username, message.data);
      if (!conn->send(Message(TAG_OK, "message broadcast to room"))) {
        return;
      }
    }

    // not a valid tag
    else {
      conn->send(Message(TAG_ERR, "invalid tag"));
    }
  }

  // cleanup
  if (room) {
    room->remove_member(user);
  }
  delete user;
}

/// SENDER AND RECEIVER IMPLEMENTATIONS (END)

namespace {

void *worker(void *arg) {

  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Info *info_ptr = static_cast<Info *>(arg);
  std::unique_ptr<Info> info(info_ptr);

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_msg;
  if (!info->conn->receive(login_msg)) {
    if (info->conn->get_last_result() == Connection::INVALID_MSG) {
      info->conn->send(Message(TAG_ERR, "invalid message"));
    } else {
      info->conn->send(Message(TAG_ERR, "couldn't receive message"));
    }
    return nullptr;
  } else if (!(login_msg.tag == TAG_SLOGIN || login_msg.tag == TAG_RLOGIN)) {
    info->conn->send(Message(TAG_ERR, "need to login first"));
    return nullptr;
  } 

  string username = login_msg.data;
  if (!info->conn->send(Message(TAG_OK, "welcome, " + username))) {
    return nullptr;
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  User *user = new User(login_msg.data);
  if (login_msg.tag == TAG_RLOGIN) {
    receiver_chat(info->conn, info->server, user);
  } else if (login_msg.tag == TAG_SLOGIN) {
    sender_chat(info->conn, info->server, user);
  }
  
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port) 
  : m_port(port), m_ssock(-1) {
  // TODO: initialize mutex
    pthread_mutex_init(&m_lock, nullptr);
  }

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  string port = to_string(m_port);
  m_ssock = open_listenfd(port.c_str());
  if (m_ssock < 0) {
    cerr << "Server socket failed to open";
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client

  while (1) {
    int clientfd = accept(m_ssock, nullptr, nullptr);
    if (clientfd < 0) {
      cerr << "Error accepting connection";
      return;
    }

    Info *info = new Info(new Connection(clientfd), this);

    pthread_t thread_id;

    if (pthread_create(&thread_id, nullptr, worker, static_cast<void *>(info)) != 0) {
      cerr << "Failed to create thread";
      return;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard guard(m_lock);
  auto i = m_rooms.find(room_name);
  
  Room *r;

  // if room does not exist, create new room and add to map
  if (i == m_rooms.end()) {
    r = new Room(room_name);
    m_rooms[room_name] = r;
  } else {
    r = i->second;
  }

  return r;
}
