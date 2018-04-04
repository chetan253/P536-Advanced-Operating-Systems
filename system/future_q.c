#include<xinu.h>
#include<future.h>

void fut_enqueue(queue *q, pid32 proc_id){
	qnode *new_proc_node = (qnode *)getmem(sizeof(struct qnode));
	qnode *node;
	new_proc_node->proc_id = proc_id;
	new_proc_node->next = NULL;	
	//In case of empty queue
	if(q->qhead->next == NULL){ 
		q->qhead->next = new_proc_node;
		return;
	}
	node = q->qhead->next;
	while(node->next != NULL)
		node = node->next;
	//appending to last
	node->next = new_proc_node;
	return;
}

pid32 fut_dequeue(queue *q){
	qnode *temp;
	//If list is null
	if(q->qhead->next == NULL){
		return 0;
	}
	temp = q->qhead->next;
	q->qhead->next = temp->next;
	return temp->proc_id;
}
