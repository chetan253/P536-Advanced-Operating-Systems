#include<xinu.h>
#include<future.h>
#include<stdio.h>
uint32 future_proc_ring(int);
uint32 future_ring(future*, future*);
uint future_prod(future* ,int);
uint future_cons(future*);
uint32 future_prod_cons();
/**
 *  * Test Futures
 *   */
uint32 future_test(int nargs, char *args[])
{

#ifndef _FUTURE_H_
  printf("no support for futures (NFUTURES not defined.)\n");
  return OK;
#endif

  if (nargs == 3 && strncmp(args[1], "future_test",11 )==0) {
	if(strncmp(args[2], "-r", 2) == 0) {
    		return future_proc_ring(1);
	    	printf("Producer/consumer process ring\n");
	}
	else if(strncmp(args[2], "-pc", 3) == 0){
		return future_prod_cons();
	}
  }
  else if(nargs == 2 && strncmp(args[1], "future_test",11)==0){
    return future_proc_ring(1);
    printf("producer/consumer process ring\n");
  }
  else {
    printf("No valid options\n");
    return(OK);
  }
  return(OK);
}

uint32 future_proc_ring(int ring){
  int future_flags = 0;
  int ring_count = 10;
  int final_val;
  int i;
  if (ring == 1) {
    future *first, *in, *out = NULL;
    first = future_alloc(future_flags);
    in = first;
    for (i=0; i < ring_count; i++) {
      out = future_alloc(future_flags);
      resume( create(future_ring, 1024, 20, "", 2, in, out) );
      in = out;
    }
    printf("master sets %d\n", ring_count);
    future_set(first, &ring_count);
    future_get(out, &final_val);
    printf("master gets %d\n", final_val);
    return(OK);
  }
  return(OK);
}

uint future_ring(future *in, future *out) {
  int val;
  future_get(in, &val);
  printf("Process %d gets %d, puts %d\n", getpid(), val, val-1);
  val--;
  future_free(in);
  future_set(out, &val);
  return OK;
}

uint32 future_prod_cons(){
	future* f_exclusive, *f_shared, *f_queue;            
        f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
        f_shared    = future_alloc(FUTURE_SHARED);
        f_queue     = future_alloc(FUTURE_QUEUE);
	resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
	resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, 1));
	sleepms(10);	
	printf("SHARED\n");
	resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
	resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, 2) );
  	resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
  	//resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
  	resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
	resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, 3) );  	
  	resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) ); 
  	//resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
	return(OK);
}

uint future_prod(future* fut,int n) {
  printf("Produced %d\n",n);
  future_set(fut, &n);
  return OK;
}

uint future_cons(future* fut) {
  int i, status;
  status = (int)future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Consumed %d\n", i);
  return OK;
}

shellcmd xsh_future_test(int nargs, char *args[]){
	future_test(nargs, args);
	return SHELL_OK;
}
