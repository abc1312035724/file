#ifndef _AXISMANAGER_H_
#define	_AXISMANAGER_H_

	#include "AxisMove.h"
	#include "usertool.h"
	#include "can_master.h"
	typedef enum
	{
		AxFlow,		//传送轴0
		AxPickZ,	//上下轴1
		AxPull,		//拉线轴2
		AxPickLF,	//左前  3
		AxPickRF,	//右前  4
		AxPickLB,	//左后  5
		AxPickRB,	//右后  6
		AxMask,		//打片轴8	
		//AxWeld,		//焊接轴7
		//AxHit,		//打料轴 8
		//AxBelt_1,	//放带轴1 7
		//AxBelt_2,	//放带轴2 8
//		AxPull_2,	//拉线2轴 8
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
