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
#include "warehouseAlarmStruct.h"
#define MEMORY_NAME_CHILD "DM1"
#define MEMORY_NAME_BOILER "DM2"
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
struct boilerStruct * DM2;
struct warehouseAlarmsStruct * DM3;
char * ptrMemName;
int coLevel,value;
/* signal handler, signal 400 for P4 , signal 100 for P1 */
sig_handler(signal_code sig)
{
	switch(sig)
	{
		case 100:
		printf("P2: System Is Ready\n");
		printf("P2: Auto-Check of CO Reading . . .\n");
/* function call to read data and send signal if danger*/
		doRead();
		break;
	}
	_os_rte();
}
doRead()
{
	num_ticks = 2000;
	while (num_ticks != 0) {
		_os_sleep( & num_ticks, & dummy_sig);
	}
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/* links to event*/
	if((errno = _os_ev_link(EVENT_NAME_BOILER,&ev_id))!=0)
	{
		printf("P1: Event Boiler not linked?\n");
		exit(errno);
	}
	if((errno=_os_ev_wait(ev_id,&value,&sig,1,1))!=0)
	{
		printf("P1: Waiting on event error");
		exit(errno);
	}
/*Critcial Section*/
/* linking the memory module */
	ptrMemName = MEMORY_NAME_BOILER;
	errno = _os_link( & ptrMemName, (mh_com * * ) & mod_head,
		(void * * ) & DM2, & type_lang, & attr_rev);
	if (errno != 0) {
		fprintf(stderr, "P1: %d: Error, Cannot link Data Boiler
			Module\n");
		_os_exit(errno);
	}
	coLevel=DM2->COlevel;
	if((errno=_os_ev_signal(ev_id,&value,0))!=0)
	{
		printf("P2: Signalling event error\n");
		exit(errno);
	}
	if((errno=_os_ev_unlink(ev_id))!=0)
	{
		printf("P2: Event unlink error ");
		exit(errno);
	}
	_os_unlink( & mod_head);
	if(coLevel>50)
	{
		coProtocol(coLevel);
	}
}
/* function to send signal */
coProtocol(coLevel)
{
	if((errno = _os_ev_link(EVENT_NAME_ALARM,&ev_id))!=0)
	{
		printf("P2: Event Alarm not linked?\n");
		exit(errno);
	}
	if((errno=_os_ev_wait(ev_id,&value,&sig,1,1))!=0)
	{
		printf("P2: Waiting on event error");
		exit(errno);
	}
/*Critcial Section*/
/* linking the memory module */
	ptrMemName = MEMORY_NAME_ALARM;
	errno = _os_link( & ptrMemName, (mh_com * * ) & mod_head,
		(void * * ) & DM3, & type_lang, & attr_rev);
	if (errno != 0) {
		fprintf(stderr, "P2: %d: Error, Cannot link Data Alarm
			Module\n");
		_os_exit(errno);
	}
	printf("P2: CO level too high is at %d%, starting Boiler CO leak
		protocol sequence\n",coLevel);
	DM3->Co2Alarm=1;
	_os_unlink( & mod_head);
	ptrMemName = MEMORY_NAME_CHILD;
	errno= _os_link(&ptrMemName,(mh_com**)&mod_head,(void
		**)&DM1,&type_lang,&attr_rev);
	if(errno!=0)
	{
		fprintf(stderr,"P2: %d: Error, Cannot link PID Data
			Module\n");
		_os_exit(errno);
	}
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/* send signal to P3 */
	_os_send(DM1->childPid[3],200);
	_os_unlink(&mod_head);
/*release event*/
	if((errno=_os_ev_signal(ev_id,&value,0))!=0)
	{
		printf("P2: Signalling event error\n");
		exit(errno);
	}
	if((errno=_os_ev_unlink(ev_id))!=0)
	{
		printf("P2: Event unlink error ");
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
	num_ticks=17000;
	while(num_ticks != 0)
	{
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}