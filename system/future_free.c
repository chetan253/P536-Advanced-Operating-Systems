// Future free
#include<xinu.h>
#include<future.f>

syscall future_free(future* f){
	//release the memory
	freemem(f, sizeof(f));
	return OK;
}
