CONTRIBUTIONS

MS1: Alan and Lauren worked together to implement the various client functions. Alan worked on receiver.cpp while Lauren worked on sender.cpp.
MS2: Alan worked on implementing the server and rooms while Lauren worked on the message queue, report, and debugging.

REPORT

Synchronization plays a crucial role in our implementation of the server. To avoid hazards like race conditions and deadlocks, 
we utilized the Guard object, which initializes a pthread_mutex_t object and handles locking and unlocking the mutex, for 
synchronization and protecting critical sections. Guard objects helps us to avoid deadlocking as the threads will automatically unlock 
when the Guard goes outside of the scope of our program. We also employed semaphores (sem_t object) to manage the MessageQueue data 
structure in a multi-threaded environment, by waiting for messages to become available (sem_timedwait), notifying availability of 
new messages (sem_post), and limiting concurrent access. We've identified the following critical sections in our code, as places where
we've used specific synchronization primitives to prevent synchronization errors:

1. Creating a new room (Server::find_or_create_room)
   - Used a Guard object to ensure that creation of a new room and modification of the map was protected, proceeding without interruption.
2. Enqueueing a message in the message queue (MessageQueue::enqueue)
   - Ensured seamless and protected addition of the message to the queue resource using a Guard object.
   - Employed a semaphore (sem_post) to signal to the waiting thread to resume previous operations after the message has been added.
3. Dequeueing a message from the message queue (MessageQueue::dequeue)
   - Ensured seamless and protected addition of the message to the queue resource using a Guard object.
   - Employed a semaphore (sem_timedwait) to instruct the current thread to wait for a specified duration while the client thread executed the operation,
      preventing data race conditions.
4. Broadcasting a message to the room (Room::broadcast_message)
   - Ensured safe transmission messages to all room members, accessing and iteration through message queue, and interruption handling, using a Guard object.
5. Adding a member to the room (Room::add_member)
   - Used a Guard object to ensure the addition of a member to the room's shared UserSet resource would proceed without concurrent modification/data races, while handling interruptions.
6. Removing a member from the room (Room::remove_member)
   - Used a Guard object to ensure the removal of a member from the room's shared UserSet resource would proceed without concurrent modification/data races, while handling interruptions.