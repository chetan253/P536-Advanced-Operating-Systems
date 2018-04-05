//Future get
#include<xinu.h>
#include<future.h>

syscall future_get(future* f, int* value){
	if(f->flag == FUTURE_EXCLUSIVE){
		//if empty current pid will be suspended
		if(f->state == FUTURE_EMPTY){
			f->state = FUTURE_WAITING;
			f->pid = getpid();
			suspend(getpid());
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
		//intmask mask = disable();
		//printf("Get called by %d\n",getpid());
		if(f->state == FUTURE_EMPTY){
			f->state = FUTURE_WAITING;
			fut_enqueue(f->get_queue, getpid());
			//printf("Get called and enquing %d\n",getpid());
			suspend(getpid());
			*value = *(f->value);
		}
		else if(f->state == FUTURE_WAITING){
  			fut_enqueue(f->get_queue, getpid());
			//printf("get called in waiting %d\n",getpid());
			suspend(getpid());
			*value = *(f->value);
		}
		else if(f->state == FUTURE_VALID){
			*value = *(f->value);
		}
		//printf("fvalue* %d\n",*(f->value));
                //printf("value %d\n", value);
		//restore(mask);
	}
	else if(f->flag == FUTURE_QUEUE){
                intmask mask = disable();
		pid32 proc_id;
                if(f->state == FUTURE_EMPTY){
                        f->state = FUTURE_WAITING;
			fut_enqueue(f->get_queue, getpid());
                        suspend(getpid());
                        *value = *(f->value);
                }
                else if(f->state == FUTURE_WAITING){
                        fut_enqueue(f->get_queue, getpid());
                        suspend(getpid());
                        *value = *(f->value);
                }
                else if(f->state == FUTURE_VALID){
			proc_id = fut_dequeue(f->set_queue);
			if(f->get_queue->qhead->next == NULL && f->set_queue->qhead->next == NULL){
                                f->state = FUTURE_EMPTY;
                        }
			resume(proc_id);
                        *value = *f->value;
                }
                //printf("value* %d\n",*value);
                //printf("value %d\n", value);
                restore(mask);
        }
	return OK;
}
