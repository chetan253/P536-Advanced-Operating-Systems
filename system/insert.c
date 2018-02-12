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
	int16	curr;			/* Runs through items in a queue*/
	int16	prev;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = firstid(q);
	while (queuetab[curr].qkey >= key) {
		curr = queuetab[curr].qnext->pid;
	}

	/* Insert process between curr node and previous node */

	prev = queuetab[curr].qprev->pid;    /* Get index of previous node   */
    	queuetab[pid].pid = pid;
    	queuetab[pid].qnext = &queuetab[curr];
    	queuetab[pid].qprev = &queuetab[prev];
    	queuetab[pid].qkey = key;
    	queuetab[prev].qnext = &queuetab[pid];
    	queuetab[curr].qprev = &queuetab[pid];
	return OK;
}
