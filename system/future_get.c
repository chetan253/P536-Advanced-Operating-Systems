//Future get
#include<xinu.h>
#include<future.h>

syscall future_get(future* f, int* value){
	if(f->flag = FUTURE_EXCLUSIVE){
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
	else if(f->flag == FUTURE_SHARED){
		intmask mask = disable();
		queue *q = f->get_queue;
		pid32 proc_id = getpid();
		if(f->state != FUTURE_VALID){
			fut_enqueue(f->get_queue, proc_id);
			suspend(proc_id);
			*value = *(f->value);		
		}
		if(f->state == FUTURE_VALID){
			*value = *(f->value);
		}
		restore(mask);
		return OK;
	}
	return OK;
}
