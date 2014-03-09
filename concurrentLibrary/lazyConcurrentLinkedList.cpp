//
//  lazyConcurrentLinkedList.cpp
//  concurrentLibrary
//
//  Created by umit on 3/12/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#include <iostream>

#include "lazyConcurrentLinkedList.h"
#include "spinLock.h"

void initLazyConcurrentLinkedList(struct list* liste)
{
    /********  init list *********/
    liste->head = (struct node*)malloc(sizeof(struct node));
    liste->tail = (struct node*)malloc(sizeof(struct node));
    liste->head->key = 0;
    liste->head->lock = false;
    liste->head->marked = false;
    liste->head->next = liste->tail;
    liste->tail->key = 0x7FFFFFFF;
    liste->tail->lock = false;
    liste->tail->marked = false;
    /*****************************/
}

bool validate(struct node* pred, struct node* curr)
{
    return !pred->marked && !curr->marked && pred->next == curr;
}

bool add(int key, struct list* list)
{
    struct node* newNode = NULL;
    struct node* pred;
    struct node* curr;
    
    while (true) {
        pred = list->head;
        curr = list->head->next;
        
        while (curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        
        lock_begin(pred);
        
        lock_begin(curr);
        
        if (validate(pred, curr)) {
            if (curr->key == key) {
                unlock(curr);
                unlock(pred);
                return false;
            }
            else {
                newNode = (struct node*)malloc(sizeof(struct node));
                newNode->key = key;
                newNode->next = curr;
                newNode->marked = false;
                newNode->lock = false;
                __sync_lock_test_and_set(&(pred->next), newNode);
                unlock(curr);
                unlock(pred);
                return true;
            }
        }
        
        lock_end(curr);
        
        lock_end(pred);
    }
}

bool remove(int key, struct list* list)
{
    struct node* pred;
    struct node* curr;
    
    while (true) {
        pred = list->head;
        curr = list->head->next;
        
        while (curr->key < key) {
            pred = curr;
            curr = curr->next;
        }
        
        lock_begin(pred);
        
        lock_begin(curr);
        
        if (validate(pred, curr)) {
            if (curr->key != key) {
                unlock(curr);
                unlock(pred);
                return false;
            } else {
                if (__sync_bool_compare_and_swap(&(curr->marked), false, true) && __sync_bool_compare_and_swap(&(pred->next), pred->next, curr->next)) {
                    unlock(curr);
                    unlock(pred);
                    return true;
                }
            }
        }
        
        lock_end(curr);
        
        lock_end(pred);
        
    }
}

bool contains(int key, struct list* list)
{
    struct node* curr = list->head;
    while (curr->key < key) {
        curr = curr->next;
    }
    
    return curr->key == key && !curr->marked;
}