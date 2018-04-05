#include<xinu.h>
#include<future.h>

syscall future_set(future* f, int* value){
	
	if(f->flag == FUTURE_EXCLUSIVE){	
		//change waiting and empty states to valid
		if(f->state == FUTURE_WAITING || f->state == FUTURE_EMPTY){
			f->state = FUTURE_VALID;
			(*f).value = value;
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
		//printf("value* %d\n",value);
		//printf("value %d\n", value);
		if(f->state == FUTURE_EMPTY){
			f->state = FUTURE_VALID;
			(*f).value = value;
		}
		else if(f->state == FUTURE_WAITING){
			f->state = FUTURE_VALID;
                        (*f).value = value;
			//printf("(*f).value* %d\n", (*f).value);
			//remove all the processes
			while(f->get_queue->qhead->next != NULL){
                                //printf("set while\n");
				proc_id = fut_dequeue(f->get_queue);
                                //printf("Resuming the procid %d\n",proc_id);
				resume(proc_id);
                        }
		}
		restore(mask);
		return OK;
	}
	else if(f->flag == FUTURE_QUEUE){
		intmask mask = disable();
                pid32 proc_id;
		if(f->state == FUTURE_EMPTY){
                        f->state = FUTURE_VALID;
                        f->pid = currpid;
			(*f).value = value;
			fut_enqueue(f->set_queue, currpid);
			suspend(currpid);
                }
                else if(f->state == FUTURE_WAITING){
      			proc_id = fut_dequeue(f->get_queue);
                        (*f).value = value;
                        resume(proc_id);
		}
		else if(f->state == FUTURE_VALID){
			f->pid = currpid;
			fut_enqueue(f->set_queue, currpid);
                        suspend(currpid);
			(*f).value = value;
		}
		restore(mask);
	}
	return OK;
}
