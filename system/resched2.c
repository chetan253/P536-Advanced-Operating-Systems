/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched2(uint16 next_state, pid16 currex_pid)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	struct procent *ptnext; // ptr to table entry of next process

	/* If rescheduling is deferred, record attempt and return */	
	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}
	
	//input state validation for 8 process states
	if(next_state < 0 || next_state > 7){
		return;
	}
	
	ptnext = &proctab[currex_pid];
	//check for currently executing process if state is PR_CURR or PR_READY 
	if (ptnext->prstate == PR_CURR) {  /* Process remains eligible */
                if (ptnext->prprio > firstkey(readylist)) {
                        return;
                }

                ptnext->prstate = PR_READY;
                insert(curexrpid, readylist, ptnext->prprio);
        }
	
	ptnext->prstate = next_state;	//assigning next state

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
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

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched2();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
