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

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

struct Info {
  Connection *conn;
  Server *server;
  ~Info() {
    delete conn;
  }
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

void chat_with_sender(Connection *conn, Server *server, User *user)
{
  Room *room = nullptr;
  while (1)
  {

    Message message;

    // try to receieve a message
    if (!conn->receive(message))
    {
      // failed to recieve a message
      if (conn->get_last_result() == Connection::INVALID_MSG || conn->get_last_result() == Connection::EOF_OR_ERROR)
      {
        conn->send(Message(TAG_ERR, "Invalid message"));
        return;
      }
      else
      {
        conn->send(Message(TAG_ERR, "Couldn't receieve message!"));
      }
    }
    else
    {
      // successfully receieved a message
      if (message.tag == TAG_ERR)
      {
        std::cerr << message.data;
        return;
      }

      // message receieve error too long
      if (message.data.length() == Message::MAX_LEN)
        conn->send(Message(TAG_ERR, "Message length exceeds max length"));

      // quit the program
      if (message.tag == TAG_QUIT)
      {
        conn->send(Message(TAG_OK, "Bye"));
        return;
      }
      // if user has not joined a room, the only command available is joining a room
      else if (room == nullptr)
      {
        if (message.tag == TAG_JOIN)
        {
          room = server->find_or_create_room(message.data);
          room->add_member(user);
          if (!conn->send(Message(TAG_OK, "joined room")))
          {
            return;
          }
        }
        else
        {
          conn->send(Message(TAG_ERR, "Must join room to send a message!"));
        }
      }
      // respond to /join [room]
      else if (message.tag == TAG_JOIN)
      {
        room->remove_member(user);
        room = server->find_or_create_room(message.data);
        room->add_member(user);
        if (!conn->send(Message(TAG_OK, "joined new room")))
        {
          return;
        }
      }
      else if (message.tag == TAG_SENDALL)
      { // respond to [message text]
        room->broadcast_message(user->username, message.data);
        if (!conn->send(Message(TAG_OK, "message sent to all in room")))
        {
          return;
        }
      }
      // respond to /leave
      else if (message.tag == TAG_LEAVE)
      {
        // de-register sender form room
        if (room != nullptr)
        {
          room->remove_member(user);
          room = nullptr;
          if (!conn->send(Message(TAG_OK, "left room")))
          {
            return;
          }
        }
        else
        {
          conn->send(Message(TAG_ERR, "Not current in room"));
        }
      }
      else
      {
        if (!conn->send(Message(TAG_ERR, "Tag is not valid")))
        {
          return;
        }
      }
    }
  }
  delete user;
  delete conn;
}

void chat_with_receiver(Connection *conn, Server *server, User *user)
{
  // terminate the loop and tear down the client thread if any message transmission fails or if quit message
  //  respond to join room
  Message message = Message();
  Room *room = nullptr;
  if (!conn->receive(message))
  {
    if (conn->get_last_result() == Connection::INVALID_MSG)
    {
      conn->send(Message(TAG_ERR, "invalid message"));
    }
    else
    {
      conn->send(Message(TAG_ERR, "couldn't receive message"));
    }
    return;
  }

  if (message.tag == TAG_JOIN)
  {
    room = server->find_or_create_room(message.data);
    room->add_member(user);
    if (!conn->send(Message(TAG_OK, "joined room")))
    {
      return;
    }
  }
  else
  {
    conn->send(Message(TAG_ERR, "invalid message, Sender needs to join a room"));
  }

  // send queued messages to receiver
  while (1)
  {

    Message *msg = user->mqueue.dequeue();

    if (msg != nullptr)
    {
      if (!conn->send(*msg))
      {
        delete msg;
        break;
      }
      delete msg;
    }
  }
  room->remove_member(user);
  return;
}

/// ^^^ OLD IMPLMENTATION 

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  struct Info *info_ptr = (Info *) arg;
  std::unique_ptr<Info> info(info_ptr);

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_message = Message();

  if (!info->conn->receive(login_message)) {
    if (info->conn->get_last_result() == Connection::INVALID_MSG) {
      info->conn->send(Message(TAG_ERR, "invalid message"));
    } else {
      info->conn->send(Message(TAG_ERR, "couldn't receive message"));
    }
    return nullptr;
  }

  if (login_message.tag != TAG_SLOGIN && login_message.tag != TAG_RLOGIN) {
    info->conn->send(Message(TAG_ERR, "User needs to login first"));
    return nullptr;
  } else if (!info->conn->send(Message(TAG_OK, "logged in as " + login_message.data))) {
    return nullptr;
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  User *user = new User(login_message.data);
  std::string username = login_message.data;

  // depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  // sender
  if (login_message.tag == TAG_SLOGIN) {
    chat_with_sender(info->conn, info->server, user);
  } else if (login_message.tag == TAG_RLOGIN) {
    chat_with_receiver(info->conn, info->server, user);
  }
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
}

Server::~Server() {
  // TODO: destroy mutex
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
}
