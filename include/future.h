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

typedef struct{
	pid32 proc_id;
	struct qnode *next;
}qnode;

typedef struct{
	qnode *qhead;
	qnode *qtail;
}queue;

typedef struct{
   int *value;		
   int flag;	
   int state; 	
   pid32 pid;
   queue *set_queue;     
   queue *get_queue;
}future;

/*Future queue funtions*/
void fut_enqueue(queue *, pid32);
pid32 fut_dequeue(queue *);

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);
 
#endif /* _FUTURE_H_ */
