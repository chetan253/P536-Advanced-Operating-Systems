#include<xinu.h>
#include<future.h>

void fut_enqueue(queue *q, pid32 proc_id){
	qnode *new_proc_node = (qnode *)getmem(sizeof(qnode));
	new_proc_node->proc_id = proc_id;
	new_proc_node->next = NULL;	
	//In case of empty queue
	if(q->qtail == NULL || q->qhead == NULL){ 
		q->qhead = q->qtail = new_proc_node;
		return;
	}
	//appending to last
	q->qtail->next = new_proc_node;
	q->qtail = q->qtail->next;
	return;
}

pid32 fut_dequeue(queue *q){
	qnode *temp;
	if(q->qhead == NULL){
		return 0;
	}
	temp = q->qhead;
	q->qhead = q->qhead->next;
	return temp->proc_id;
}
