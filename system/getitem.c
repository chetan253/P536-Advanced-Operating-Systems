/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	//pid32	head;

	if (isempty(q)) {
		return EMPTY;
	}

	//head = queuehead(q);
	return getitem(firstid(q));
}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	//pid32 tail;
	
	if (isempty(q)) {
		return EMPTY;
	}

	//tail = queuetail(q);
	return getitem(lastid(q));
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
	)
{
	//pid32	prev, next;

	qid16 qid = 0;
    	struct qentry *prevnode, *currnode, *removenode;
	removenode = NULL;
    	if(proctab[pid].prstate == PR_READY){
        	qid = readylist;
    	}
    	else{
        	qid = sleepq;
    	}
    	currnode = &queuetab[queuehead(qid)];
	prevnode = currnode;	
    	while(currnode->qnext != NULL){
        	if(currnode->pid == pid){
            		removenode = currnode; // to free the space of this node
            		prevnode->qnext = currnode->qnext;
	            	currnode->qnext->qprev = prevnode;
            		break;
        	}
		prevnode = currnode;
        	currnode =  currnode->qnext;
	}
	freemem((char *)removenode, sizeof(struct qentry));
    	return pid;
}
