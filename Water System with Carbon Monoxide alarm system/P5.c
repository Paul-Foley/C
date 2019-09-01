#include <stdio.h>
#include <errno.h>
#include <modes.h>
#include <stdlib.h>
#include <signal.h>
#include <types.h>
#include <string.h>
#include <cglob.h>
#include <module.h>
#include <process.h>
#include <dexec.h>
#include <types.h>
#include <module.h>
#include <memory.h>
#include "ChildPID.h"
#include "BoilerStruct.h"
#define MEMORY_NAME_CHILD "DM1"
#define MEMORY_NAME_BOILER "DM2"
/* Declaring variables */
char *ptr_mem_name;
u_int16 attr_rev,type_lang,perm,mem_size;
mh_com mod_head;
signal_code DummySignal,dummy_sig;
u_int32 SleepValue;
char *ptrMemName;
error_code err;
u_int32 num_ticks;
signal_code sig;
event_id ev_id;
struct childPID * DM1;
struct boilerStruct * DM2;
char * ptrMemName;
int boilerswitch,value;
/* signal handler, signal 300 for P3 */
sig_handler(signal_code sig)
{
	switch(sig)
	{
		case 400:
		printf("P5: Pump begining to shutdown! \n");
/* function call to read data and send signal if danger*/
		doBoilerShutdown();
		break;
	}
	_os_rte();
}
doBoilerShutdown()
{
	num_ticks = 2000;
	while (num_ticks != 0) {
		_os_sleep( & num_ticks, & dummy_sig);
	}
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/*linking event */
	if((errno = _os_ev_link(EVENT_NAME_BOILER,&ev_id))!=0)
	{
		printf("P5: Event Boiler not linked?\n");
		exit(errno);
	}
	if((errno=_os_ev_wait(ev_id,&value,&sig,1,1))!=0)
	{
		printf("P5: Waiting on event error");
		exit(errno);
	}
/*Critcial Section*/
/* linking the memory module */
	ptrMemName = MEMORY_NAME_BOILER;
	errno = _os_link( & ptrMemName, (mh_com * * ) & mod_head,
		(void * * ) & DM2, & type_lang, & attr_rev);
	if (errno != 0) {
		fprintf(stderr, "P5: %d: Error, Cannot link Data Boiler
			Module\n");
		_os_exit(errno);
	}
	strcpy(DM2->pumpStatus,"Off");
	_os_unlink( & mod_head);
	if((errno=_os_ev_signal(ev_id,&value,0))!=0)
	{
		printf("P5: Signalling event error\n");
		exit(errno);
	}
	if((errno=_os_ev_unlink(ev_id))!=0)
	{
		printf("P5: Event unlink error ");
		exit(errno);
	}
	safetyShutdownProtocol();
}
/* function to send signal */
safetyShutdownProtocol()
{
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/* linking the memory module */
	ptrMemName = MEMORY_NAME_CHILD;
	errno= _os_link(&ptrMemName,(mh_com**)&mod_head,(void
		**)&DM1,&type_lang,&attr_rev);
	if(errno!=0)
	{
		fprintf(stderr,"P5: %d: Error, Cannot link PID Data
			Module\n");
		_os_exit(errno);
	}
/* send signal to P6 */
	_os_send(DM1->childPid[6],500);
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
	num_ticks=20000;
	while(num_ticks != 0)
	{
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}