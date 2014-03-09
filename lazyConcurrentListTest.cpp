//
//  lazyConcurrentListTest.cpp
//  concurrentLibrary
//
//  Created by umit on 3/13/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#include <iostream>
#include <pthread.h>
#include "lazyConcurrentLinkedList.h"
#include "fineGrainedHeapPrioQueue.h"
#include "concurrentTests.h"

struct list liste;
int keyCount = 1;

#define NUMBER_OF_THREAD 350
#define MAX_NUMBER       5000

void* threadCode(void* arg)
{
    int number;
    int how;
    int random;
    
    for (int i = 0; i < 200; ++i) {
        random = rand();
        how = random % 3;
        
        switch (how) {
            case 0:{
                number = random % MAX_NUMBER;
                if (add(number, &liste))
                    printf("adding number %d\n", ++number);
                break;
            }
            case 1:{
                number = random % MAX_NUMBER;
                if (remove(number, &liste))
                    printf("removing number %d\n", ++number);
                break;
            }
            case 2:{
                number = random % MAX_NUMBER;
                if (contains(number, &liste))
                    printf("contains number %d\n", ++number);
                break;
            }
            default:
                break;
        }
    }
    
    return NULL;
}

void lazyConcurrentLinkListTest()
{
    initLazyConcurrentLinkedList(&liste);
    
    pthread_t tidArray[NUMBER_OF_THREAD];
    
    for (int i = 0; i < NUMBER_OF_THREAD; ++i) {
        pthread_create(&tidArray[i], NULL, threadCode, NULL);
    }
    
    for (int i = 0; i < NUMBER_OF_THREAD; ++i) {
        pthread_join(tidArray[i], NULL);
    }
    
    struct node* tempHead = liste.head;
    while (tempHead != NULL) {
        printf("%d\n", tempHead->key);
        tempHead = tempHead->next;
    }
}