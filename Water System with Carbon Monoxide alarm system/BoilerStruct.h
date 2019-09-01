/*
Gas Boiler Sysytem Contains sesnsors and actuators that are
common in a modern Boiler
*/
#define MESS_SZ_BL 30
struct boilerStruct
{
	int boilerSwitch;
	int waterTemp;
	char waterLevel[4];
	char pumpStatus[3];
	int heatingElement;
	int acidLevel;
	int COlevel;
};
#define MEMORY_SIZE_BOILER sizeof(struct boilerStruct)
#define EVENT_NAME_BOILER "boiler_event" /* Event Name
used for Semaphores */
warehouseAlarmStruct.h
/*
Alarm Sysytem (Goes off depending on levels of e.g. Co2)
*/
#define MESS_SZ_ALM 30
struct warehouseAlarmsStruct
{
	int Co2Alarm;
	int GasLeakAlarm;
	int FireAlarm;
	int ExtractionFans;
};
#define MEMORY_SIZE_ALARM sizeof(struct
warehouseAlarmsStruct)
#define EVENT_NAME_ALARM "alarm_event" /* Event Name
used for Semaphores */