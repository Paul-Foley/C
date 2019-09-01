/*
 Data Declaration for sharing data in an array between processes
 Holds the process ID's of the forked process for inter-
 communication between different processes
*/
#define MESS_SZ 30
struct MemData
{
int childPid[5]; /* Array of size 5, will assign P1->[1], P2->[2] and
so on (Not good practice but its easier to understand due to the naming of the programs) */
};
#define MEMORY_SIZE sizeof(struct MemData)