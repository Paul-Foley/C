#include <stdio.h>
#include <errno.h>
#include <process.h>
#include <dexec.h>
#include <types.h>
#include <cglob.h>
#include <module.h>
#include <memory.h>
#include "BoilerStruct.h"
#include "ChildPID.h"
#include "warehouseAlarmStruct.h"
/* Memory Module Name */
#define MEMORY_NAME_PID "DM1"
#define MEMORY_NAME_STRUCT "DM2"
#define MEMORY_NAME_ALARM "DM3"
main(int argc, char *argv[], char **envp)
{
	u_int16 attr_rev,type_lang,perm,mem_size;
	mh_com mod_head;
	signal_code DummySignal;
	u_int32 SleepValue;
	event_id ev_id;
/* Declaring memeory module with pointer */
	struct childPID *DM1;
	struct boilerStruct *DM2;
	struct warehouseAlarm *DM3;
/* Declaring Variables */
	error_code err,errno;
	process_id child_pid1;
	process_id child_pid2;
	process_id child_pid3;
	process_id child_pid4;
	process_id child_pid5;
	process_id child_pid6;
	process_id child_pid7;
	status_code p1Stat;
	status_code p2Stat;
	status_code p3Stat;
	status_code p4Stat;
	status_code p5Stat;
	status_code p6Stat;
	status_code p7Stat;
	char *child_argv1[]={"P1","-e","-a",0};
	char *child_argv2[]={"P2","-e","-a",0};
	char *child_argv3[]={"P3","-e","-a",0};
	char *child_argv4[]={"P4","-e","-a",0};
	char *child_argv5[]={"P5","-e","-a",0};
	char *child_argv6[]={"P6","-e","-a",0};
	char *child_argv7[]={"P7","-e","-a",0};
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/* Permissions to read and write to memory module */
	perm= MP_OWNER_READ|MP_OWNER_WRITE;
/* Creates the memory Module that will be open to use */
	mem_size = MEMORY_SIZE_CHILDPID;
	if(errno =
		_os_datmod(MEMORY_NAME_PID,mem_size,&attr_rev,&type_la
			ng,perm, (void **)&DM1,(mh_data**)&mod_head)!=0)
	{
		fprintf(stderr,"STARTUP> Error, Cannot create Memory Module
			for Child PID\n");
		_os_exit(errno);
	}
	mem_size = MEMORY_SIZE_BOILER;
	if(errno =
		_os_datmod(MEMORY_NAME_STRUCT,mem_size,&attr_rev,&ty
			pe_lang,perm, (void **)&DM2,(mh_data**)&mod_head)!=0)
	{
		fprintf(stderr,"STARTUP> Error, Cannot create Memory Module
			for Boiler\n");
		_os_exit(errno);
	}
	mem_size = MEMORY_SIZE_ALARM;
	if(errno =
		_os_datmod(MEMORY_NAME_ALARM,mem_size,&attr_rev,&typ
			e_lang,perm, (void **)&DM3,(mh_data**)&mod_head)!=0)
	{
		fprintf(stderr,"STARTUP> Error, Cannot create Memory Module
			for Alarms\n");
		_os_exit(errno);
	}
	if((errno = _os_ev_creat(1,-1,perm, &ev_id,
		EVENT_NAME_BOILER,1,MEM_ANY))!=0)
	{
		printf("Error creating event Boiler event\n");
	}
	if((errno = _os_ev_creat(1,-1,perm, &ev_id,
		EVENT_NAME_ALARM,1,MEM_ANY))!=0)
	{
		printf("Error creating event Alarm event\n");
	}
/* Begins forking P1, P2, P3, P4, P5, P6, P7 */
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv1[0], child_argv1, envp,
		0 , &child_pid1, 0, 0)!=0))
	{
		printf("Error1 P1 1! \n");
	}
/* Assigns the child PID to value in array in memory module */
	DM1->childPid[1]=child_pid1;
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv2[0], child_argv2, envp,
		0 , &child_pid2, 0, 0)!=0))
		{ printf("Error1 P2 ! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[2]=child_pid2;
if( err=(_os_exec(_os_fork, 0 , 3 , child_argv3[0], child_argv3, envp,
	0 , &child_pid3, 0, 0)!=0))
	{printf("Error1 P3 1! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[3]=child_pid3;
if( err=(_os_exec(_os_fork, 0 , 3 , child_argv4[0], child_argv4, envp,
	0 , &child_pid4, 0, 0)!=0))
	{printf("Error1 P4 1! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[4]=child_pid4;
if( err=(_os_exec(_os_fork, 0 , 3 , child_argv5[0], child_argv5, envp,
	0 , &child_pid5, 0, 0)!=0))
	{printf("Error1 P5 1! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[5]=child_pid5;
if( err=(_os_exec(_os_fork, 0 , 3 , child_argv6[0], child_argv6, envp,
	0 , &child_pid6, 0, 0)!=0))
	{printf("Error1 P6 1! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[6]=child_pid6;
if( err=(_os_exec(_os_fork, 0 , 3 , child_argv7[0], child_argv7, envp,
	0 , &child_pid7, 0, 0)!=0))
	{printf("Error1 P7 1! \n");
}
/* Assigns the child PID to value in array in memory module */
DM1->childPid[7]=child_pid7;
SleepValue=0;
while(1)
{
/* Infinite wait */
	_os_sleep(&SleepValue,&DummySignal);
}
}