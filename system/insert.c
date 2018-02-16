/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	//int16	curr;			/* Runs through items in a queue*/
	//int16	prev;			/* Holds previous node index	*/
	struct qentry *curr, *prev, *newnode
	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	while (queuetab[curr].qkey >= key) {
		curr = queuetab[curr].qnext->pid;
	}

	//struct qentry *curr, *prev, *newnode;                   /* Runs through items in a queue*/
    	curr = &queuetab[queuehead(q)];
    	while (curr->qkey >= key) {
       		curr = curr->qnext;
    	}
    	prev = curr->qprev;
    	newnode = (struct qentry*)malloc(sizeof(struct qentry));
    	newnode->pid = pid;
    	newnode->qkey = key;
    	newnode->qnext = curr;
    	curr->qprev = newnode;
    	newnode->qprev = prev;
	prev->qnext = newnode;

	/* Insert process between curr node and previous node */

	/*prev = queuetab[curr].qprev->pid;
    	queuetab[pid].pid = pid;
    	queuetab[pid].qnext = &queuetab[curr];
    	queuetab[pid].qprev = &queuetab[prev];
    	queuetab[pid].qkey = key;
    	queuetab[prev].qnext = &queuetab[pid];
    	queuetab[curr].qprev = &queuetab[pid];
	return OK;*/
}
