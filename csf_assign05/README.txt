CONTRIBUTIONS

MS1: Alan and Lauren worked together to implement the various client functions. Alan worked on receiver.cpp while Lauren worked on sender.cpp.
MS2: Alan worked largely on implementing the server and Lauren worked on the report and debugging

REPORT
We utilized the Guard object, which initializes a pthread_mutex_t object, for synchronization and 
employed a sem_t object to manage the MessageQueue data structure. Guard objects helps us to avoid 
deadlocking as the threads will immediately unlock when Guard goes outside of the scope of our program

Critical Sections:
1. Enqueueing a message in the message queue (MessageQueue::enqueue)
   - Ensured the seamless addition of the message to the queue via a Guard object
   - Employed a semaphore (sem_post) to signal to the waiting thread to resume previous operations after the message has been added.

2. Dequeueing a message from the message queue (MessageQueue::dequeue)
   - Pre-critical section: Used a semaphore (sem_timedwait) to instruct the current thread to wait for a specified duration 
        while the client thread executed the operation.
   - Used a Guard object to secure the uninterrupted removal of the message from the queue.

3. Adding a member to the room (Room::add_member)
   - Employed a Guard object to guarantee the addition of a member to the room's UserSet without interruptions.

4. Removing a member from the room (Room::remove_member)
   - Utilized a Guard object to ensure the uninterrupted removal of a member from the room's UserSet.

5. Broadcasting a message to the room (Room::broadcast_message)
   - Ensured the uninterrupted transmission of messages to all room members through the use of a Guard object.

6. Finding or creating a room (Server::find_or_create_room)
   - Used a Guard object to ensure the discovery or creation of a room without interruptions from other threads.