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
u_int16 attr_rev, type_lang, perm, mem_size;
mh_com mod_head;
signal_code DummySignal;
u_int32 SleepValue,value;
struct childPID * DM1;
struct boilerStruct * DM2;
char * ptrMemName;
error_code err;
u_int32 num_ticks;
signal_code dummy_sig;
signal_code sig;
event_id ev_id;
main() {
/* declaring variables for use */
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
	num_ticks = 2000;
	while (num_ticks != 0) {
		_os_sleep( & num_ticks, & dummy_sig);
	}
/* linking to event*/
	if((errno = _os_ev_link(EVENT_NAME_BOILER,&ev_id))!=0)
	{
		printf("P1: Event Boiler not linked?\n");
		exit(errno);
	}
/*waits if other process has event*/
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
/*assigning values*/
	printf("P1: Gathering Sensor Information ...\n");
	DM2->boilerSwitch=1;
	DM2->waterTemp=43;
	strcpy(DM2->waterLevel,"Low");
	strcpy(DM2->pumpStatus,"On");
	DM2->heatingElement=0;
	DM2->acidLevel=1;
	DM2->COlevel=76;
	printf("Boiler On/Off: %d\n",DM2->boilerSwitch);
	printf("Water temperature: %d\n",DM2->waterTemp);
	printf("Water Level: %s\n",DM2->waterLevel);
	printf("Pump On/Off: %s \n",DM2->pumpStatus);
	printf("Heating Element On/Off: %d\n",DM2->heatingElement);
	printf("Acidity Reading: %d\n",DM2->acidLevel);
	printf("CO Levels: %d\n",DM2->COlevel);
	_os_unlink( & mod_head);
	ptrMemName = MEMORY_NAME_CHILD;
	errno= _os_link(&ptrMemName,(mh_com**)&mod_head,(void
		**)&DM1,&type_lang,&attr_rev);
	if(errno!=0)
	{
		fprintf(stderr,"P1: %d: Error, Cannot link Data Module\n");
		_os_exit(errno);
	}
/* send signal 100 to P2 which is stored in [2] in the memory
module */
	_os_send(DM1 -> childPid[2], 100);
	_os_unlink( & mod_head);
	if((errno=_os_ev_signal(ev_id,&value,0))!=0)
	{
		printf("P1: Signalling event error\n");
		exit(errno);
	}
	if((errno=_os_ev_unlink(ev_id))!=0)
	{
		printf("P1: Event unlink error ");
		exit(errno);
	}
/* Semaphore realsesd */
	num_ticks=17000;
	while(num_ticks != 0)
	{
		_os_sleep(&num_ticks, &dummy_sig);
	}
	_os_exit(0);
}