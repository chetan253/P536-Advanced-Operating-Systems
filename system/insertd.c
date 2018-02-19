/* insertd.c - insertd */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insertd  -  Insert a process in delta list using delay as the key
 *------------------------------------------------------------------------
 */
status	insertd(			/* Assumes interrupts disabled	*/
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Delay from "now" (in ms.)	*/
	)
{
	//int32	next;			/* Runs through the delta list	*/
	//int32	prev;			/* Follows next through the list*/
        struct qentry *next, *prev, *newnode, *tail; /*structs to manipulate the queue*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	
    	prev = &queuetab[queuehead(q)];
    	next = prev->qnext;
	tail = &queuetab[queuetail(q)];
    	while ((next != tail) && (next->qkey <= key)) {
        	key -= next->qkey;
        	prev = next;
        	next = next->qnext;
    	}
	//inserting the process node
    	newnode = (struct qentry *)getmem(sizeof(struct qentry));
    	newnode->qkey = key;
    	newnode->pid = pid;
    	newnode->qnext = next;
    	newnode->qprev = prev;
    	next->qprev = newnode;
    	prev->qnext = newnode;
    	if (next != tail) {
     		next->qkey -= key;
   	 }

	return OK;
}
