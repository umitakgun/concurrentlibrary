//
//  spinLock.h
//  concurrentLibrary
//
//  Created by umit on 3/14/12.
//  Copyright (c) 2012 umitak. All rights reserved.
//

#ifndef concurrentLibrary_spinLock_h
#define concurrentLibrary_spinLock_h

#define unlock(node)     if ((node) != NULL) {                                                                  \
                            if (node->lock == true)                                                             \
                            {                                                                                   \
                                while (!__sync_bool_compare_and_swap(&((node)->lock), true, false)) {           \
                                    printf("unlock fail\n");                                                    \
                                }                                                                               \
                            }                                                                                   \
                         }

#define lock_begin(node) if (((node) != NULL) && (__sync_bool_compare_and_swap(&((node)->lock), false, true))) {

#define lock_end(node)       unlock(node);\
                         }

#endif
