/*
 Data Declaration for sharing data in an array between processes
 Holds the process ID's of the forked process for inter-
 communication between different processes
*/
#define MESS_SZ 30
struct childPID
{
int childPid[8]; /* Array of size 5, will assign P1->[1], P2->[2] and
so on */
};
#define MEMORY_SIZE_CHILDPID sizeof(struct childPID)
