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
#include "warehouseAlarmStruct.h"
#define MEMORY_NAME_CHILD "DM1"
#define MEMORY_NAME_ALARM "DM3"
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
struct warehouseAlarmsStruct * DM3;
char * ptrMemName;
int coLevel,value;
/* signal handler, signal 600 for P6 */
sig_handler(signal_code sig)
{
	switch(sig)
	{
		case 600:
/* function call*/
		ActivateGasAlarms();
		break;
	}
	_os_rte();
}
ActivateGasAlarms()
{
	num_ticks = 2000;
	while (num_ticks != 0) {
		_os_sleep( & num_ticks, & dummy_sig);
	}
/*links event*/
	if((errno = _os_ev_link(EVENT_NAME_ALARM,&ev_id))!=0)
	{
		printf("P7: Event Alarm not linked?\n");
		exit(errno);
	}
	if((errno=_os_ev_wait(ev_id,&value,&sig,1,1))!=0)
	{
		printf("P7: Waiting on event error");
		exit(errno);
	}
/*Critcial Section*/
/* linking the memory module */
	ptrMemName = MEMORY_NAME_ALARM;
	errno = _os_link( & ptrMemName, (mh_com * * ) & mod_head,
		(void * * ) & DM3, & type_lang, & attr_rev);
	if (errno != 0) {
		fprintf(stderr, "P7: %d: Error, Cannot link Data Alarm
			Module\n");
		_os_exit(errno);
	}
	DM3->GasLeakAlarm=1;
	_os_unlink( & mod_head);
	printf("P7: WARNING \n");
	printf("P7: GAS LEAK IN BOILER \n");
	printf("P7: FOLLOW PROCEDURE AND EVACUATE
		BUILDING\n");
	printf("P7: ALARM SYSTEM ACTIVATED\n");
	printf("*Alarm Noises\n");
/*release event */
	if((errno=_os_ev_signal(ev_id,&value,0))!=0)
	{
		printf("P7: Signalling event error\n");
		exit(errno);
	}
	if((errno=_os_ev_unlink(ev_id))!=0)
	{
		printf("P7: Event unlink error ");
		exit(errno);
	}
}
main()
{
/* declaring variables */
	error_code err;
	int num_ticks;
	signal_code dummy_sig;
	if((err =_os_intercept(sig_handler, _glob_data))!=0)
		exit(err);
	num_ticks=27000;
	while(num_ticks != 0)
	{
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}