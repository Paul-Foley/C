#include <stdio.h>
#include <errno.h>
#include <process.h>
#include <dexec.h>
#include <types.h>
#include <cglob.h>
#include <module.h>
#include "MemData.h"
#define MEMORY_NAME "DM1" /* Memory Module Name */
main(int argc, char *argv[], char **envp)
{
	u_int16 attr_rev,type_lang,perm,mem_size;
	mh_com mod_head;
	signal_code DummySignal;
	u_int32 SleepValue;
struct MemData *DM1; /* Declaring memeory module with
pointer */
/* Declaring Variables */
	error_code err,errno;
	process_id child_pid1;
	process_id child_pid2;
	process_id child_pid3;
	process_id child_pid4;
	status_code p1Stat;
	status_code p2Stat;
	status_code p3Stat;
	status_code p4Stat;
	char *child_argv1[]={"P1","-e","-a",0};
	char *child_argv2[]={"P2","-e","-a",0};
	char *child_argv3[]={"P3","-e","-a",0};
	char *child_argv4[]={"P4","-e","-a",0};
	mem_size = MEMORY_SIZE;
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
/* Permissions to read and write to memory module */
	perm= MP_OWNER_READ|MP_OWNER_WRITE;
/* Creates the memory Module that will be open to use */
	if(errno =
		_os_datmod(MEMORY_NAME,mem_size,&attr_rev,&type_lang,pe
			rm, (void **)&DM1,(mh_data**)&mod_head)!=0)
	{
		fprintf(stderr,"Error, Cannot create Memory Module\n");
		printf(errno+" Error NO\n");
		_os_exit(errno);
	}
/* Begins forking P1, P2, P3, P4 */
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv1[0], child_argv1, envp,
		0 , &child_pid1, 0, 0)!=0))
		printf("Error1 P1 1! \n");
/* Assigns the child PID to value in array in memory module */
	DM1->childPid[1]=child_pid1;
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv2[0], child_argv2, envp,
		0 , &child_pid2, 0, 0)!=0))
		printf("Error1 P2 ! \n");
/* Assigns the child PID to value in array in memory module */
	DM1->childPid[2]=child_pid2;
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv3[0], child_argv3, envp,
		0 , &child_pid3, 0, 0)!=0))
		printf("Error1 P3 1! \n");
/* Assigns the child PID to value in array in memory module */
	DM1->childPid[3]=child_pid3;
	if( err=(_os_exec(_os_fork, 0 , 3 , child_argv4[0], child_argv4, envp,
		0 , &child_pid4, 0, 0)!=0))
		printf("Error1 P4 1! \n");
/* Assigns the child PID to value in array in memory module */
	DM1->childPid[4]=child_pid4;
/* Can individually wait for children to finish but using infinite
loop instead
if(err = (_os_wait(&child_pid0, &p0Stat)!=0))
printf("Error P1 2 \n");
if(err = (_os_wait(&child_pid1, &p1Stat)!=0))
 printf("Error P2 2 \n");
if(err = (_os_wait(&child_pid2, &p2Stat)!=0))
 printf("Error P3 2 \n");
if(err = (_os_wait(&child_pid3, &p3Stat)!=0))
 printf("Error P4 2 \n");
*/
	SleepValue=0;
	while(1)
	{
/* Infinite wait */
		_os_sleep(&SleepValue,&DummySignal);
	}
}