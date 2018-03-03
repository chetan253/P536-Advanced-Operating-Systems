// Future allocation
#include<xinu.h>
#include<future.h>
future* future_alloc(int future_flag){
	future* f;
	//Allocating memory for new empty future
	f = (future *)getmem(sizeof(future));
	f->flag = future_flag;
	f->state = FUTURE_EMPTY;
	return f;
}
