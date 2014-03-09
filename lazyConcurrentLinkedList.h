//
//  lazyConcurrentLinkedList.h
//  concurrentLibrary
//
//  Created by umit on 3/12/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#ifndef concurrentLibrary_lazyConcurrentLinkedList_h
#define concurrentLibrary_lazyConcurrentLinkedList_h

#include <stdio.h>

struct node {
    struct node* next;
    bool lock;
    bool marked;
    int key;
};

struct list {
    struct node* head;
    struct node* tail;
};


void initLazyConcurrentLinkedList(struct list* liste);

bool validate(struct node* pred, struct node* curr);
bool add(int key, struct list* list);
bool remove(int key, struct list* list);
bool contains(int key, struct list* list);

#endif
