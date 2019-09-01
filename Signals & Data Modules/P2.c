#include <stdio.h>
#include <errno.h>
#include <modes.h>
#include <stdlib.h>
#include <signal.h>
#include <types.h>
#include <string.h>
#include <cglob.h>
#include "MemData.h"
#define MEMORY_NAME "DM1"
/* Declaring variables */
char *ptr_mem_name;
u_int16 attr_rev,type_lang,perm,mem_size;
mh_com mod_head;
signal_code DummySignal;
u_int32 SleepValue;
struct MemData *DM1;
char *ptrMemName;
error_code err;
u_int32 num_ticks;
/* signal handler, signal 400 for P4 , signal 100 for P1 */
sig_handler(signal_code sig)
{
	switch(sig)
	{
		case 400: printf("P2: Signal From P4 received\n");
/* For recieving signal from P4 */
		_os_exit(0);
		break;
		case 100: printf("P2: Signal From P1 received \n");
/* function call to send signal to P3 */
		doSend();
		break;
	}
	_os_rte();
}
/* function to send signal */
doSend()
{
/* assigning variables */
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
	ptr_mem_name=MEMORY_NAME;
/* Link memory module */
	errno= _os_link(&ptr_mem_name,(mh_com**)&mod_head,(void
		**)&DM1,&type_lang,&attr_rev);
	if(errno!=0)
	{
		fprintf(stderr," %d: Error, Cannot link Data Module\n");
		_os_exit(errno);
	}
	num_ticks=500;
	_os_sleep(&num_ticks, &DummySignal);
/* send signal to P3 */
	_os_send(DM1->childPid[3],200);
	_os_unlink(&mod_head);
}
main()
{
/* declaring variables */
	error_code err;
	int num_ticks;
	signal_code dummy_sig;
	if((err =_os_intercept(sig_handler, _glob_data))!=0)
		exit(err);
	num_ticks=40000;
	while(num_ticks != 0)
	{
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}