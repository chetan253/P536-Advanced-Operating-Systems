/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	struct qentry	*tail, *prev, *newnode;		/* Tail & previous node indexes	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	tail = &queuetab[queuetail(q)];
	//enqueue new process using pointers
    	prev = tail->qprev;

    	newnode = (struct qentry *)getmem(sizeof(struct qentry));

    	newnode->pid = pid;
    	newnode->qnext = tail;
    	newnode->qprev = prev;

    	prev->qnext = newnode;
    	tail->qprev = newnode;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirst(q);
	return pid;
}
