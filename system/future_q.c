#include<xinu.h>
#include<future.h>

void fut_enqueue(struct queue *q, pid32 proc_id){
	qnode *new_proc_node = (qnode *)getmem(sizeof(struct qnode));
	new_proc_node->proc_id = proc_id;
	new_proc_node->next = NULL;	
	qnode *node;
	//In case of empty queue
	/*if(q->qhead->next == NULL){ 
		q->qhead->next = new_proc_node;
		return;
	}*/
	node = q->qhead;
	while(node->next != NULL){
		node = node->next;
	}
	//appending to last
	node->next = new_proc_node;
	//printf("Exit\n");
	return;
}

pid32 fut_dequeue(struct queue *q){
	qnode *temp;
	pid32 proc_id;
	//If list is null
	if(q->qhead->next == NULL){
		return 0;
	}
	temp = q->qhead->next;
	proc_id = temp->proc_id;
	
	q->qhead->next = temp->next;
	
	freemem((char *)temp, sizeof(*temp));
	return proc_id;
}
