/*Ref: Xinu Operating System textbook and Discussion with Anurag Jain*/
#include<process_ring.h>

/*Polling*/
process polling(int proc_index, int proc_count, int round_count){
	volatile int i = 0;
	volatile int last_printed = -1;
	while(i < round_count){
		if(inbox[proc_index] != last_printed && inbox[proc_index] >= 0){
			printf("Ring Element %d : Round %d : Value %d\n", proc_index, i, inbox[proc_index]);
			last_printed = inbox[proc_index];	//if number is already printed dont print again 
			value--;	// This will help in to completely execute child process by keeping parent process alive.
			inbox[(proc_index+1)%proc_count] = inbox[proc_index] - 1;
			i++;
		}
	}
	return SHELL_OK;
}

/*Synchronization via semaphores*/	
void sync(int proc_index, int proc_count, int round_count){
	volatile int i = 0;
	while(i < round_count){
		wait(sema[proc_index]);	//wait for process to execute 
		
		//critical section
		printf("Ring Element %d : Round %d : Value %d\n", proc_index, i, inbox[proc_index]);
		inbox[(proc_index+1) % proc_count] = inbox[proc_index] - 1;	//set next inbox value to decremented value
		
		signal(sema[(proc_index + 1) % proc_count]);		//signal next process to start its execution
		i++;
	}
	signal(sync_completed);	//signal process that it has completed the printing of numbers and parent process can stop thereafter
}
