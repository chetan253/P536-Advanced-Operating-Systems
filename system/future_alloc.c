// Future allocation
#include<xinu.h>
#include<future.h>
future* future_alloc(int future_flag){
	future* f;
	f = (future *)malloc(sizeof(future));
	f->flag = future_flag;
	f->state = FUTURE_EMPTY;
	return f;
}
