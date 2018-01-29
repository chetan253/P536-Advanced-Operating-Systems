#include<xinu.h>
#include<stdlib.h>

int32 n = 0;

process prod(sid32 consumer, sid32 producer){
	int32 i;
	for(i = 0; i < 10; i++){
		wait(consumer);
		n++;
		signal(producer);
	}
	return OK;
}

process cons(sid32 consumer, sid32 producer){
	int32 i;
	for(i = 0; i < 10; i++){
		wait(producer);
		printf("n is :  %d \n", n);
		signal(consumer);
	}
	return OK;
}

process msg_consumer(pid32 parent_pid){
  int32 last = -1; 
  while (last < 3){
    last = receive();
    printf("consumed: %d\n", last);
  }
  send(parent_pid, OK);
  return OK;
}                    

process msg_producer(pid32 consume_pid,
                     pid32 parent_pid){
  int32 last = -1; 
  while (last < 3){
    last = last + 1;
    printf("produced: %d\n", last);
    send(consume_pid, last);
  }
  send(parent_pid, OK);
  return OK;
} 

shellcmd xsh_tester(int nargs, char *args[]){
/*	n = 0;
	sid32 producer, consumer;
	consumer = semcreate(0);
	producer = semcreate(1);
	
	resume(create(prod, 1024, 20, "producer", 2, consumer, producer));
	resume(create(cons, 1024, 20, "consumer", 2, consumer, producer));*/
	
	pid32 parent_pid  = getpid();
 	pid32 consume_pid = create(msg_consumer, 1024, 20, "consumer", 1, parent_pid);
  	resume(consume_pid);
  	resume(create(msg_producer, 1024, 20, "producer", 2, consume_pid, parent_pid));
	receive();
  	receive();
	return 1;
}
