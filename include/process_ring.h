/*process_ring.h- Header file for xsh_process_ring.c and /apps/process_ring.h*/

#ifndef process_ring
#define process_ring
#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern volatile sid32 sema[64];
extern volatile sid32 sync_completed;
extern volatile int inbox[64];
extern volatile int value;
extern process polling(int, int, int);
extern void sync(int, int, int);
#endif
