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


shellcmd xsh_tester(int nargs, char *args[]){
	n = 0;
	sid32 producer, consumer;
	consumer = semcreate(0);
	producer = semcreate(1);
	
	resume(create(prod, 1024, 20, "producer", 2, consumer, producer));
	resume(create(cons, 1024, 20, "consumer", 2, consumer, producer));
	return 1;
}
