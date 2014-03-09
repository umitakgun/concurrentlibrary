//
//  fineGrainedPriorityQueueTest.cpp
//  concurrentLibrary
//
//  Created by umit on 3/18/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include "concurrentTests.h"
#include "fineGrainedHeapPrioQueue.h"


void fineGrainedPriorityQueueTest()
{
    struct fineGrainedHeap* heap = NULL;
    initFineGrainedHeapPrioQueue(&heap, 20);
    add(NULL, 1, heap);
    add(NULL, 3, heap);
    add(NULL, 4, heap);
    add(NULL, 7, heap);
    add(NULL, 8, heap);
    add(NULL, 5, heap);
    add(NULL, 12, heap);
    add(NULL, 14, heap);
    add(NULL, 10, heap);
    add(NULL, 4, heap);
    
    removeMin(heap);
    removeMin(heap);
    
    for (int i=0; i<heap->next; ++i) {
        if (NODE(heap, i) != NULL)
            printf("%d %p \n", NODE(heap, i)->score, NODE(heap, i)->ownerId );  
    }
}