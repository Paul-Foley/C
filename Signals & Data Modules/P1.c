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
u_int16 attr_rev, type_lang, perm, mem_size;
mh_com mod_head;
signal_code DummySignal;
u_int32 SleepValue;
struct MemData * DM1;
char * ptrMemName;
error_code err;
u_int32 num_ticks;
signal_code dummy_sig;
/* signal handler, signal 400 for P4 */
sig_handler(signal_code sig) {
	switch (sig) {
		case 400:
		printf("P1: Signal From P4 received\n");
		_os_exit(0);
 /* once signal is recieved end process */
		break;
	}
 /* return back to the main */
	_os_rte();
}
main() {
 /* declaring variables for use */
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
 /* when signal comes, go to handler */
	if ((err = _os_intercept(sig_handler, _glob_data)) != 0) {
		exit(err);
	}
	ptrMemName = MEMORY_NAME;
 /* linking the memory module */
	errno = _os_link( & ptrMemName, (mh_com * * ) & mod_head,
		(void * * ) & DM1, & type_lang, & attr_rev);
	if (errno != 0) {
		fprintf(stderr, " %d: Error, Cannot link Data Module\n");
		_os_exit(errno);
	}
	num_ticks = 500;
	_os_sleep( & num_ticks, & dummy_sig);
 /* send signal 100 to P2 which is stored in [2] in the memory
module */
	_os_send(DM1 -> childPid[2], 100);
	num_ticks = 40000;
	while (num_ticks != 0) {
		_os_sleep( & num_ticks, & dummy_sig);
	}
	_os_unlink( & mod_head);
	_os_exit(0);
}