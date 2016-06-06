
#ifndef _SYSTEM_DEF_H
#define _SYSTEM_DEF_H

#define SYSTEM_PROFILE		".//process.ini"

#define MAX_CAR_CNT			12
#define MAX_CYCLE_CNT		5
#define MAX_TIME_CNT		5*60

#define CAC_BY_CYCLE		1
#define CAC_BY_TIME			2

#define CAR_ST_INIT			0				/* 车辆图标蓝色               */
#define CAR_ST_RUNNING		1				/* 车辆图标黄色               */
#define CAR_ST_OUTFLOW		2				/* 车辆计时/计圈已到,图标红色 */

typedef struct CarInfo
{
	CString	carName;		/* 车辆名称 */
	CString carSeqIDstr;	/* 车辆rfid */
	int		carSeqIDint;	/* 车辆rfid */

	int		carCycleCnt;	/* 车辆圈数 */
	int		carTimeSec;		/* 车辆时间 */

	int		carCurState;	/* 车辆状态 */

	int		carOnline;		/* 在线状态 */
}CarInfo;

typedef struct SystemDef
{
	int		carCacType;		/* 赛道计量方式 */
	int		carMaxCycle;	/* 赛道最大圈数 */
	int		carMaxTime;		/* 赛道最大时长 */
}SystemDef;

#endif