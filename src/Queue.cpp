#include "../includes/Queue.h"

#pragma unmanaged
void writeLog(std::string type, std::string directory, std::string name) {
  std::cout
    << type
    << ": "
    << directory
    << "/"
    << name
    << std::endl;
}

Queue::Queue() {
  OPA_Queue_init(&mQueue);
  OPA_store_int(&mNumEvents, 0);
}

Queue::~Queue() {
  while(!OPA_Queue_is_empty(&mQueue)) {
    EventNode *node;

    OPA_Queue_dequeue(&mQueue, node, EventNode, header);

    delete node->event;
    delete node;
  }
}

void Queue::clear() {
  while(!OPA_Queue_is_empty(&mQueue)) {
    EventNode *node;

    OPA_decr_int(&mNumEvents);
    OPA_Queue_dequeue(&mQueue, node, EventNode, header);

    delete node->event;
    delete node;
  }
}

int Queue::count() {
  return OPA_load_int(&mNumEvents);
}

Event *Queue::dequeue() {
  if (!OPA_Queue_is_empty(&mQueue)) {
    EventNode *node;

    OPA_decr_int(&mNumEvents);
    OPA_Queue_dequeue(&mQueue, node, EventNode, header);

    Event *event = node->event;
    delete node;

    return event;
  }
  return NULL;
}

void Queue::enqueue(EventType event, std::string directory, std::string fileA, std::string fileB) {
  EventNode *node = new EventNode;

  OPA_Queue_header_init(&node->header);

  node->event = new Event;
  node->event->type = event;
  node->event->directory = directory;
  node->event->fileA = fileA;
  node->event->fileB = fileB;

  OPA_Queue_enqueue(&mQueue, node, EventNode, header);
  OPA_incr_int(&mNumEvents);

  switch(event) {
  case CREATED:
    writeLog("CREATED", directory, name);
    break;
  case DELETED:
    writeLog("DELETED", directory, name);
    break;
  case MODIFIED:
    writeLog("MODIFIED", directory, name);
    break;
  case RENAMED:
    writeLog("RENAME A", directory, name);
    writeLog("RENAME B", directory, renamedName);
    break;
  }
}
