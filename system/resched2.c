/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched2(uint16 next_state)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */	
	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}
	
	//input state validation for 8 process states
	if(next_state < 0 || next_state > 7){
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	ptold->prstate = next_state;

	if (ptold->prstate == PR_CURR || ptold->prstate == PR_READY) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/

#ifdef MMU
	FlushTLB();
	setPageTable();
#endif/*MMU*/

	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */
	
	return;
}
