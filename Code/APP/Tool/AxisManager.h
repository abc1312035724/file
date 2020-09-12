#ifndef _AXISMANAGER_H_
#define	_AXISMANAGER_H_

	#include "AxisMove.h"
	#include "usertool.h"
	#include "can_master.h"
	typedef enum
	{
		AxFlow,		//������0
		AxPickZ,	//������1
		AxPull,		//������2
		AxPickLF,	//��ǰ  3
		AxPickRF,	//��ǰ  4
		AxPickLB,	//���  5
		AxPickRB,	//�Һ�  6
		AxMask,		//��Ƭ��8	
		//AxWeld,		//������7
		//AxHit,		//������ 8
		//AxBelt_1,	//�Ŵ���1 7
		//AxBelt_2,	//�Ŵ���2 8
//		AxPull_2,	//����2�� 8
	}AxName;
	
	
typedef struct
{
    u8 execute;
    u8 step;
    u8 done;
    u8 count;
    u8 index;
    u32 tm;
    u8 stepBuffer;

    u32 Axis;
    u32 pos;
    u32 Spd;
} ExJogStepPara;	
	


	extern s32 AxAbsUnit(u32 Num, float SpdRatio, float Pos);
	extern s32 AxRelUnit(u32 Num, float SpdRatio, float Pos);
	extern s32 AxGohome(u32 Num);
	extern s32 AxStop(u32 Num);
	extern s32 AxSta(u32 Num);
	extern s32	AxInpSta(u32 Num);
	extern s32	AxCurrPos(u32 Num);
	extern s32	AxSpeedReload(u32 Num, float SpdRatio);
	
	extern void ExJogStep(void);

#endif
