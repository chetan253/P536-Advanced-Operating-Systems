// Future allocation
#include<xinu.h>
#include<future.h>
future* future_alloc(int future_flag){
	future* f;
	//Allocating memory for new empty future
	f = (future *)getmem(sizeof(future));
	if(future_flag == FUTURE_EXCLUSIVE){
		f->flag = future_flag;
		f->state = FUTURE_EMPTY;
	}
	if(future_flag == FUTURE_SHARED){
                qnode *node;
		
		f->flag = future_flag;
                f->state = FUTURE_EMPTY;
		
		queue *q = (queue *)getmem(sizeof(struct queue));
		
		f->get_queue = q;
		node = (qnode *)getmem(sizeof(struct qnode));
		node->proc_id = 0;
		node->next = NULL;
		f->get_queue->qhead = node;
        }
	if(future_flag == FUTURE_QUEUE){
		qnode *node1 = (qnode *)getmem(sizeof(struct qnode));
		qnode *node2 = (qnode *)getmem(sizeof(struct qnode));
                
		f->flag = future_flag;
                f->state = FUTURE_EMPTY;
		
		queue *q1 = (queue *)getmem(sizeof(struct queue));
		queue *q2 = (queue *)getmem(sizeof(struct queue));
		
		f->get_queue = q1;
		f->set_queue = q2;

		node1->proc_id = 0;
                node1->next = NULL;
		node2->proc_id = 0;
                node2->next = NULL;
		f->get_queue->qhead = node1;
		f->set_queue->qhead = node2;
        }

	return f;
}
