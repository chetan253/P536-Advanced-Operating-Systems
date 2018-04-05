#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3     

typedef struct qnode{
	pid32 proc_id;
	struct qnode *next;
}qnode;

typedef struct queue{
	struct qnode *qhead;
}queue;

typedef struct futent
{
   int *value;		
   int flag;	
   int state; 	
   pid32 pid;
   struct queue *set_queue;     
   struct queue *get_queue;

}future;

void fut_enqueue(struct queue *, pid32);
pid32 fut_dequeue(struct queue*);

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);
 
#endif /* _FUTURE_H_ */
