#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

using std::string;

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // initialize the mutex
  pthread_mutex_init(&lock, nullptr);
}

Room::~Room() {
  // destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // add User to the room
  Guard guard(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  // remove User from the room
  Guard guard(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // send a message to every (receiver) User in the room
  Guard guard(lock);
  for (auto user: members) { 
    if (user->username != sender_username) {
      string txt = this->get_room_name() + ":" + sender_username + ":" + message_text;
      Message *msg = new Message(TAG_DELIVERY, txt);
      user->mqueue.enqueue(msg);
    } 
  }
}
