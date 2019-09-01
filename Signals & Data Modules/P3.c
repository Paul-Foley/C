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
/* Signal handler */
sig_handler(signal_code sig)
{
	switch(sig)
	{
		case 400: printf("P3: Signal From P4 received\n");
		_os_exit(0);
		break;
/* Once signal 400 from P4 is recieved, end process */
		case 200: printf("P3: Signal From P2 received \n");
/* Call function to send signal */
		doSend();
		break;
	} -
	_os_rte();
}
/* Function to send signal */
doSend()
{
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
	ptr_mem_name=MEMORY_NAME;
	errno= _os_link(&ptr_mem_name,(mh_com**)&mod_head,(void
		**)&DM1,&type_lang,&attr_rev);
	if(errno!=0)
	{
		fprintf(stderr," %d: Error, Cannot link Data Module\n");
		_os_exit(errno);
	}
	num_ticks=500;
	_os_sleep(&num_ticks, &DummySignal);
/* Send Signal 300 to P4 */
	_os_send(DM1->childPid[4],300);
	_os_unlink(&mod_head);
}
main()
{
	error_code err;
	int num_ticks;
	signal_code dummy_sig;
	if((err =_os_intercept(sig_handler, _glob_data))!=0)
		exit(err);
	num_ticks=40000;
	while(num_ticks != 0)
	{
/* Wait for num_ticks to reach 0 */
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}