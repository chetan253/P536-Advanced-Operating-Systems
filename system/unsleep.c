/* unsleep.c - unsleep */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  unsleep  -  Internal function to remove a process from the sleep
 *		    queue prematurely.  The caller must adjust the delay
 *		    of successive processes.
 *------------------------------------------------------------------------
 */
status	unsleep(
	  pid32		pid		/* ID of process to remove	*/
        )
{
	struct qentry *node;
	intmask	mask;			/* Saved interrupt mask		*/
        struct	procent	*prptr;		/* Ptr to process' table entry	*/
	qid16 	qid;
        struct qentry	*pidnext;		/* ID of process on sleep queue	*/
					/*   that follows the process	*/
					/*   which is being removed	*/

	mask = disable();

	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Verify that candidate process is on the sleep queue */

	prptr = &proctab[pid];
	if ((prptr->prstate!=PR_SLEEP) && (prptr->prstate!=PR_RECTIM)) {
		restore(mask);
		return SYSERR;
	}
	
	if(proctab[pid].prstate == PR_READY){
        	qid = readylist;
    	}
    	else{
        	qid = sleepq;
    	}
	/* Increment delay of next process if such a process exists */
	node = &queuetab[queuehead(qid)];
	while(node->pid != pid){
		node = node->qnext;
	}
	pidnext = node->qnext;
	//pidnext = queuetab[pid].qnext->pid;
	if (pidnext->pid < NPROC) {
		pidnext->qkey += node->qkey;
		//queuetab[pidnext].qkey += queuetab[pid].qkey;
	}

	getitem(pid);			/* Unlink process from queue */
	restore(mask);
	return OK;
}
