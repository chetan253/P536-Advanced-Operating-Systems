#include<xinu.h>
#include<future.h>

syscall future_set(future* f, int* value){
	//change waiting and empty states to valid
	if(f->state == FUTURE_WAITING || f->state == FUTURE_EMPTY){
		f->state = FUTURE_VALID;
		f->value = value;
		resume(f->pid);
		return OK;
	}
	else{
		return SYSERR;
	}
}
