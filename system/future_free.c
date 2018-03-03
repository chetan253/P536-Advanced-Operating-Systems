// Future free
#include<xinu.h>
#include<future.h>

syscall future_free(future* f){
	//release the memory of allocated future
	freemem((char* )f, sizeof(f));
	return OK;
}
