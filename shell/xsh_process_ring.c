#include <process_ring.h>

volatile int inbox[64];
volatile int value;
volatile sid32 sema[64];
volatile sid32 sync_completed;

shellcmd xsh_process_ring(int nargs, char *args[]){
	int proc_count = 2;
	int round_count = 3;
	int flag = 0;	//default polling
	int i = 0;
	uint32 start, end, time;
//	char *endp;
//	int a = 0;
//	a = strtol(args[2], &endp, 10);
//	printf("a is %d", a);
	if(nargs < 2){
                fprintf(stderr, "%s: Invalid Arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",args[0]);
		return SHELL_ERROR;
        }

	for(i = 0; i < nargs; i++){
		if(strncmp(args[1], "--help", 7) == 0){
        	        printf("Usage: %s \n\t -p Number of Processes<0-64> Default 2\n", args[0]);
               		printf("\t -r Number of Rounds, Default 3\n");
	                printf("\t -i <poll or sync>\n");
			printf("\t Example: %s -p 1 -r 3 -i sync\n", args[0]);
	                printf("Description: Counts down the number (processes * rounds - 1) to zero by passing to each process\n");
	                return 1;
	        }

		if(strncmp(args[i], "-p", 3) == 0){
			if(!(i+1 < nargs)){
				printf("-p flag expected an argument\n");
			        return SHELL_ERROR;
			}
			proc_count = atoi(args[i+1]);
			if(proc_count < 0){
				printf("-p flag expected an positive integer\n");
				return SHELL_ERROR;
			}
			else if(!(0 <= proc_count && proc_count <= 64)){
                                printf("-p flag expected a number between 0 - 64\n");
                                return SHELL_ERROR;
			}
			i += 1;
		}
		else if(strncmp(args[i], "-r", 3) == 0){
			if(!(i+1 < nargs)){ 
                                printf("-r flag expected an argument\n");
                                return SHELL_ERROR;
                       	}
                        round_count = atoi(args[i+1]);
                        if(round_count < 0){ 
                                printf("-r flag expected an positive integer\n");
                                return SHELL_ERROR;
                       	}
                        else if(!(0 <= round_count && round_count <= 100)){ 
                                printf("-r flag expected a number between 0 - 100\n");
                                return SHELL_ERROR;
                       	}
                        i += 1;
		}
		else if(strncmp(args[i], "-i", 3) == 0){
			if(!(i+1 < nargs)){
                               	printf("-r flag expected an argument\n");
                                return SHELL_ERROR;
                        }
			if(strncmp(args[i+1], "sync", 5) == 0){
				flag = 1;
				i += 1; 
			}
			else if(strncmp(args[i+1], "poll", 5) == 0){
				flag = 0;
				i += 1;
			}
			else{
				printf("-i flag expected an argument <poll or sync>\n");
                                return SHELL_ERROR;
			}
			
		}
	}
	
	gettime(&start);	
	inbox[0] = (proc_count * round_count) - 1;	 
	value = inbox[0];
	//execute polling(flag 0) or synchronization(flag 1)
	printf("Number of processes: %d\n", proc_count);
	printf("Number of rounds: %d\n", round_count);
	if(flag == 0){
		printf("Polling\n");
		for(i = 0 ; i < proc_count; i++){
			resume(create(polling, 1024, 20, "polling",3, i, proc_count, round_count));
		}
		while(value != -1);	//Execute the code until the value is decremented to zero.
	}
	else if(flag == 1){
		printf("Synchnronization via semaphores\n");
		for(i = 0; i < proc_count; i++){
			sema[i] = semcreate(0);	
		}
		signal(sema[0]);	//signal the very first semaphore to start execution
		for(i = 0; i < proc_count; i++){
			resume(create(sync, 1024, 20, "synchronization", 3, i, proc_count, round_count));
		}
		for(i = 0; i < proc_count; i++){
			wait(sync_completed);	//wait for child process to complete.
			semdelete(sema[i]);	//then delete the created semaphore
		}
	}
	gettime(&end);
	time = end - start;
	printf("Elasped Seconds: %d\n", time);
	return SHELL_OK;
}
