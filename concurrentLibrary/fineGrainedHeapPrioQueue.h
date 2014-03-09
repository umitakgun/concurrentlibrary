//
//  fineGrainedHeapPrioQueue.h
//  concurrentLibrary
//
//  Created by umit on 3/13/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#ifndef concurrentLibrary_fineGrainedHeapPrioQueue_h
#define concurrentLibrary_fineGrainedHeapPrioQueue_h

#include <pthread.h>
#include "spinLock.h"

enum Status {
    EMPTY = 0,
    AVAILABLE = 1,
    BUSY = 2
    };

struct heapNode {
    Status tag;
    int score; // priority
    void *item;
    pthread_t ownerId;
    bool lock;
};

struct fineGrainedHeap {
    bool heapLock;
    int next;
    int capacity;
    int length;
    struct heapNode *(*heap)[];
};

#define heapLock_begin(heap) if ((heap != NULL) && (__sync_bool_compare_and_swap(&(heap->heapLock), false, true))) {

#define heapLock_end(heap)       heapUnlock(heap);\
                             }

#define NODE(pq, node) (*(pq->heap))[node]

void initFineGrainedHeapPrioQueue(struct fineGrainedHeap **heap, int capacity);
void add(void* item, int score, struct fineGrainedHeap* heap);
void* removeMin(struct fineGrainedHeap* heap);

#endif
