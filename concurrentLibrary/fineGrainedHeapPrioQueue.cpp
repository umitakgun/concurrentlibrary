//
//  fineGrainedHeapPrioQueue.cpp
//  concurrentLibrary
//
//  Created by umit on 3/13/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#include <iostream>

#include "fineGrainedHeapPrioQueue.h"

static int ROOT = 1;

static int NO_ONE = -1;

typedef struct heapNode* (heapPtr)[];

void initFineGrainedHeapPrioQueue(struct fineGrainedHeap **heap, int capacity)
{
    //allocate heap
    (*heap) = (struct fineGrainedHeap*)malloc(sizeof(struct fineGrainedHeap));

    //initialize heap structure
    (*heap)->heapLock = false;
    (*heap)->next = ROOT;
    (*heap)->capacity = capacity;
    (*heap)->length = 0;
    
    //init heap array
    (*heap)->heap = (struct heapNode*(*)[])malloc(capacity * (sizeof(struct heapNode*)));
    memset((*heap)->heap, 0, capacity * (sizeof(struct heapNode*)));
}

void initHeapNode(struct heapNode* node, void* item, int score)
{
    node->item = item;
    node->lock = true;
    node->score = score;
    node->tag = BUSY;
    node->ownerId = pthread_self();
}

void heapUnlock(struct fineGrainedHeap* heap)
{
    if (heap != 0) {
        while (!__sync_bool_compare_and_swap(&(heap->heapLock), true, false)) {
            printf("heap unlock fail\n");
        };
    }
}

bool amIOwner(struct heapNode* node)
{
    return node->ownerId == pthread_self();
}

void swapNodes(struct heapNode** lhs, struct heapNode** rhs)
{
    __sync_val_compare_and_swap(rhs,*rhs,__sync_val_compare_and_swap(lhs,*lhs,*rhs));
}

void add(void* item, int score, struct fineGrainedHeap* heap)
{
    int child = 0;
    int parent = 0;
    int oldChild = 0;
    
    heapLock_begin(heap);
    
    child = __sync_fetch_and_add(&(heap->next), 1);
    
    NODE(heap, child) = (struct heapNode*) malloc(sizeof(struct heapNode));

    if (NODE(heap, child)) {
        
        lock_begin( NODE(heap, child) );
        initHeapNode(NODE(heap, child), item, score);
        __sync_add_and_fetch(&(heap->length),1);
        lock_end( NODE(heap, child) );
        
        heapLock_end(heap);
    }
    else {
        heapUnlock(heap);
        return;
    }
    
    while (child > ROOT) {
        parent = child / 2;
        
        lock_begin(NODE(heap, parent));
        lock_begin(NODE(heap, child));
        
        oldChild = child;
        if ((NODE(heap, parent)->tag == AVAILABLE) && amIOwner(NODE(heap, child)) )
        {
            if (NODE(heap, child)->score < NODE(heap, parent)->score)
            {
                swapNodes(&NODE(heap, child), &NODE(heap, parent));
                child = parent;
            }
            else {
                NODE(heap, child)->tag = AVAILABLE;
                NODE(heap, child)->ownerId = (pthread_t) NO_ONE;
                unlock(NODE(heap, child));
                unlock(NODE(heap, parent));
                return;
            }
        }
        else if (!amIOwner(NODE(heap, child))) {
            child = parent;
        }
        
        lock_end(NODE(heap, oldChild));
        lock_end(NODE(heap, parent));
    }
    
    if (child == ROOT) {
        lock_begin(NODE(heap, ROOT));
        
        if (amIOwner(NODE(heap, ROOT))) {
            NODE(heap, ROOT)->tag = AVAILABLE;
            NODE(heap, child)->ownerId = (pthread_t) NO_ONE;
        }
        
        lock_end(NODE(heap, ROOT));
    }
    
}

void* removeMin(struct fineGrainedHeap* heap)
{
    int bottom = 0;
    int child = 0;
    int parent = 0;
    int left = 0, right = 0;
    void* item;
    
    heapLock_begin(heap);
    
    bottom = __sync_sub_and_fetch(&(heap->next), 1);
    
    lock_begin(NODE(heap, bottom));
    lock_begin(NODE(heap, ROOT));
    
    heapLock_end(heap);
    
    item = NODE(heap, ROOT)->item;
    
    NODE(heap, ROOT)->tag = EMPTY;
    NODE(heap, ROOT)->ownerId = (pthread_t)NO_ONE;
    
    swapNodes(&NODE(heap, bottom) , &NODE(heap, ROOT));
    
    lock_end(NODE(heap, bottom));
    
    if (NODE(heap, ROOT)->tag == EMPTY) {
        unlock(NODE(heap, ROOT));
        return item;
    }
    
    child = 0;
    parent = ROOT;
    
    while (parent < heap->length / 2) {
        left = parent * 2;
        right = (parent * 2) + 1;
        
        lock_begin(NODE(heap, left));
        lock_begin(NODE(heap, right));
        
        if (NODE(heap, left)->tag == EMPTY) {
  
            unlock(NODE(heap, right));
            unlock(NODE(heap, left));
            break;
            
        }else if ( (NODE(heap, right)->tag == EMPTY) || (NODE(heap, left)->score < NODE(heap, right)->score) ) {
            
            unlock(NODE(heap, right));
            child = left;
            
        }else {
            
            unlock(NODE(heap, left));
            child = right;
            
        }
        
        if (NODE(heap, child)->score < NODE(heap, parent)->score) {
            
            swapNodes(&NODE(heap, parent), &NODE(heap, child));
            unlock(NODE(heap, parent));
            parent = child;
            
        } else {
            
            unlock(NODE(heap, child));
            break;
            
        }
        
        lock_end(NODE(heap, right));
        lock_end(NODE(heap, left));
        
    }
    
    unlock(NODE(heap, parent));
    
    lock_end(NODE(heap, ROOT));
    
    return item;
}
