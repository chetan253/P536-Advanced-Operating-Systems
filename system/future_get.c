//Future get
#include<xinu.h>
#include<future.h>

syscall future_get(future* f, int* value){
	//if empty current pid will be suspended
	if(f->state == FUTURE_EMPTY){
		f->state = FUTURE_WAITING;
		f->pid = currpid;
		suspend(currpid);
		*value = *(f->value);
		return OK;
	}
	//if valid we must set the next value so it will be printed in future at next call
	if(f->state == FUTURE_VALID){
		*value = *(f->value);
		resume(f->pid);
		f->state = FUTURE_EMPTY;

		return OK;
	}
	else{
		return SYSERR;
	}
}
