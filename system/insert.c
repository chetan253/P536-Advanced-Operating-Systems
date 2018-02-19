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
	struct qentry *curr, *prev, *newnode;
	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

    	curr = &queuetab[queuehead(q)];
	curr = curr->qnext;
    	while (curr->qkey >= key) {
       		curr = curr->qnext;
    	}
	//Non-FIFO implementation
    	prev = curr->qprev;
    	newnode = (struct qentry *)getmem(sizeof(struct qentry));
    	newnode->pid = pid;
    	newnode->qkey = key;
    	newnode->qnext = curr;
    	curr->qprev = newnode;
    	newnode->qprev = prev;
	prev->qnext = newnode;

	return OK;
}
