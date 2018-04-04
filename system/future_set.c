#include<xinu.h>
#include<future.h>

syscall future_set(future* f, int* value){
	
	if(f->flag == FUTURE_EXCLUSIVE){	
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
	else if(f->flag == FUTURE_SHARED){
		intmask mask = disable();
		pid32 proc_id;
		queue *q = f->get_queue;
		if(f->state == FUTURE_EMPTY){
			f->state = FUTURE_VALID;
			f->value = *value;
		}
		else if(f->state == FUTURE_VALID){
			//Only one thread can call future set
			restore(mask);
			return SYSERR;
		}
		else if(f->state == FUTURE_WAITING){
			while(q->qtail != NULL){
                                proc_id = fut_dequeue(q);
                                resume(proc_id);
                        }

		}
		restore(mask);
		return OK;
	}
	else if(f->flag == FUTURE_QUEUE){
		
	}
	return OK;
}
