#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd xsh_hello(int nargs, char *args[]){
	//if arguments passed correctly then display o/p else display help option or appropriate message
	if(nargs == 2){
		if(strncmp(args[1], "--help", 7) != 0){
			printf("Hello %s, Welcome to the world of Xinu!! \n",args[1]);
			return 1;
		}
		else
		{
			printf("Usage: %s <string>\n\n", args[0]);
			printf("Description: Displays message\n");
			return 1;
		}
	}
	else if(nargs < 2){
		fprintf(stderr, "%s: invalid argument\n",args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n", args[0]);
	}
	else if(nargs > 2){
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",args[0]);
	}
	return 0;
}
