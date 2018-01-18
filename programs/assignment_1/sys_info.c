#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int main(int nargs, char* args[]){
	pid_t procid;
	int fd[2];
	char buffer[100];
	pipe(fd);
	if(nargs < 2){
		printf("%s: Invalid argument\n", args[0]);
		printf("Try '%s --help' for more information\n", args[0]);
		return 1;
	}
	else if(nargs > 2){
		printf("%s: Too many arguments\n", args[0]);
		printf("Try '%s --help' for more information\n", args[0]);
		return 1;
	}
	else if(nargs == 2){
		if(strncmp(args[1], "--help", 7) == 0){
			printf("Usage: %s <Path to another program>\n", args[0]);
			printf("Description: Displays PIDs of Parent and Child Processes and executes the program supplied as command argument\n");
		}
		else{
			procid = fork();
			if(procid == 0){
				printf("Child Pid = %d\n", getpid());	//print child Pid
				close(fd[1]);				//Child closing write side of pipe
				read(fd[0], buffer, sizeof(buffer));	// reading data from pipe and storing in temp buffer
				
				/* Check the provided argument and execute the corresponding command using excl() function */

				if(strcmp(args[1], "/bin/date") == 0)
					execl("/bin/date","date", 0,0);
				else if(strcmp(args[1], "/bin/uname") == 0)
					execl("/bin/uname", "uname", 0, 0);
				else if(strcmp(args[1], "/bin/hostname") == 0)
					execl("/bin/hostname", "hostname", 0, 0);
				else if(strcmp(args[1], "/bin/echo") == 0)
					execl("/bin/echo", "echo", "Hello World!", 0);
				else{
					printf("%s: Invalid argument\n", args[0]);
			                printf("Try '%s --help' for more information\n", args[0]);
				}
			}
			else if(procid > 0){
				printf("Parent Pid = %d\n",getpid());	// print parent pid
				close(fd[0]);				// Parent closing read side
				write(fd[1], args[1], strlen(args[1]));	// writing data to pipe
				wait(NULL);				// wait until child process completes reading
			}	
			else if(procid < 0){
				printf("Fork error");
				return -1;
			}
		}
	}
}
