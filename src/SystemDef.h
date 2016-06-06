
#ifndef _SYSTEM_DEF_H
#define _SYSTEM_DEF_H

#define SYSTEM_PROFILE		".//process.ini"

#define MAX_CAR_CNT			12
#define MAX_CYCLE_CNT		5
#define MAX_TIME_CNT		5*60

#define CAC_BY_CYCLE		1
#define CAC_BY_TIME			2

#define CAR_ST_INIT			0				/* ����ͼ����ɫ               */
#define CAR_ST_RUNNING		1				/* ����ͼ���ɫ               */
#define CAR_ST_OUTFLOW		2				/* ������ʱ/��Ȧ�ѵ�,ͼ���ɫ */

typedef struct CarInfo
{
	CString	carName;		/* �������� */
	CString carSeqIDstr;	/* ����rfid */
	int		carSeqIDint;	/* ����rfid */

	int		carCycleCnt;	/* ����Ȧ�� */
	int		carTimeSec;		/* ����ʱ�� */

	int		carCurState;	/* ����״̬ */

	int		carOnline;		/* ����״̬ */
}CarInfo;

typedef struct SystemDef
{
	int		carCacType;		/* ����������ʽ */
	int		carMaxCycle;	/* �������Ȧ�� */
	int		carMaxTime;		/* �������ʱ�� */
}SystemDef;

#endif