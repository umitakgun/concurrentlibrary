//
//  reentrantLock.h
//  concurrentLibrary
//
//  Created by umit on 3/19/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#ifndef concurrentLibrary_reentrantLock_h
#define concurrentLibrary_reentrantLock_h

struct reentrantLock {
    //    bool lock;
    pthread_t owner;
    int holdCount;
};

void initReentrantLock(struct spinLock* lock);
void lockReentrant(struct spinLock* lock);
void unlockReentrant(struct spinLock* lock);

#endif
