#include "AxisManager.h"
s32 InpVal[8] = {0,50,0,10,10,10,10,0};
s32 RecordPos[8];
s32	AxAbsUnit(u32 Num, float SpdRatio, float Pos)
{
    if(Num<PULS_NUM)
	{
		SetAxisSpdRadio(Num, SpdRatio);	//设置速度
		RecordPos[Num] = Pos;
		return HZ_AxMoveAbs(Num,ConverToPulse(Num, Pos));		
	}
     else
	{
		RecordPos[Num] = Pos;
		 HZ_ExAxMoveAbs(Num - PULS_NUM,ConverToPulse(Num, Pos));
		return 0;		
	 }		 

}

s32	AxRelUnit(u32 Num, float SpdRatio, float Pos)
{
	if(Num<PULS_NUM)
	{
		SetAxisSpdRadio(Num, SpdRatio);	//设置速度
		RecordPos[Num] = GSR.AxisPosition[Num]+Pos;
		return HZ_AxMoveRel(Num,ConverToPulse(Num, Pos));
	}
	else
	{
		RecordPos[Num] = GSR.AxisPosition[Num]+Pos;
		HZ_ExAxMoveRel(Num - PULS_NUM,ConverToPulse(Num, Pos));	
		return 0;		
	}
}

s32	AxGohome(u32 Num)
{
	if(Num<PULS_NUM)
	{
		SetAxisSpdRadio(Num, 100);	//设置速度
		return HZ_AxHome(Num);
	}
	else
	{
		HZ_ExAxHome(Num - PULS_NUM);
		return 0;
	}
}

s32 AxStop(u32 Num)
{
    if(Num<PULS_NUM)
	{
		SetAxisSpdRadio(Num, 100);	//设置速度
		return HZ_AxStop(Num);
	}
	else
	{
		HZ_ExAxStop(Num - PULS_NUM);
		return 0;
	}
}

s32	AxSta(u32 Num)
{
//	if(HZ_AxGetStatus(Num) == AXSTA_READY && LIMIT_GetSta(Num) == ON)
//	{
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}
	if(Num<PULS_NUM)
	{
		return HZ_AxGetStatus(Num);
	}
	else
	{
		return HZ_ExAxGetStatus(Num - PULS_NUM);
	}	

}	

s32	AxSpeedReload(u32 Num, float SpdRatio)
{
    if(Num>PULS_NUM)
        return -1;
	AxisSpdRadioReload(Num,SpdRatio);
    return 1;
}

s32	AxCurrPos(u32 Num)
{
//	if(HZ_AxGetStatus(Num) == AXSTA_READY && LIMIT_GetSta(Num) == ON)
//	{
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}
	if(Num<PULS_NUM)
	{
		return HZ_AxGetCurPos(Num);
	}
	else
	{
		return HZ_ExAxGetCurPos(Num - PULS_NUM);
	}
}

void AxSetCurrPos(u32 Num,float pos)
{
//	if(HZ_AxGetStatus(Num) == AXSTA_READY && LIMIT_GetSta(Num) == ON)
//	{
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}
	if(Num<PULS_NUM)
	{
		HZ_AxSetCurPos(Num,pos);
	}
	else
	{
		HZ_AxSetCurPos(Num,pos);
	}
}


//轴提前到位
s32	AxInpSta(u32 Num)
{
	if(HZ_AxGetStatus(Num) == AXSTA_READY || abs(RecordPos[Num] - GSR.AxisPosition[Num]) < InpVal[Num])
	{
		return 0;
	}
	return 1;
}


ExJogStepPara ExJogStepData = {0};
//扩展卡分步发送轴动
void ExJogStep()
{
    //轴停止且设备为停止状态写一次参数
//    if(GUR.RunStatus == STOP
//            && UserGetStatus(FeedAxis) == 0
//            && UserGetStatus(Take_1_Axis) == 0
//            && UserGetStatus(Take_2_Axis) == 0 && Data.ExSendFlag == 0)
//    {
//        for(int i = 0; i < 5; i++)
//        {
//            HZ_ExAxSetPara(i, GSS.AxisData[i + PULS_NUM].StartSpeed, GSS.AxisData[i + PULS_NUM].Acctime, \
//                           GSS.AxisData[i + PULS_NUM].StartSpeed + __fabs(GSS.AxisData[i + PULS_NUM].RunSpeed - GSS.AxisData[i + PULS_NUM].StartSpeed), \
//                           GSS.AxisData[i + PULS_NUM].Dectime, GSS.AxisData[i + PULS_NUM].EndSpeed, \
//                           GSS.AxisData[i + PULS_NUM].HomeSpeedFast, GSS.AxisData[i + PULS_NUM].HomeSpeedSlow, \
//                           GSS.AxisData[i + PULS_NUM].HomeOffset, 0, 0);
//        }
//        Data.ExSendFlag = 1;
//    }

    if(ExJogStepData.execute == 1 && ExJogStepData.step == 0)
    {
        ExJogStepData.step = 1;
        ExJogStepData.count = 0;
    }
    if(GUR.RunStatus == RUN && ExJogStepData.execute == 1)
    {
        PARAINIT(ExJogStepData);
    }
    switch(ExJogStepData.step)
    {
    case 1://发送参数	
		HZ_ExAxSetPara(ExJogStepData.Axis - PULS_NUM,\
					GSS.axis[ExJogStepData.Axis].AxSpd.startspeed, GSS.axis[ExJogStepData.Axis].AxSpd.acctime,500,GSS.axis[ExJogStepData.Axis].AxSpd.dectime,GSS.axis[ExJogStepData.Axis].AxSpd.endspeed,\
					GSS.axis[ExJogStepData.Axis].Axhomecfg.homespeedfast,GSS.axis[ExJogStepData.Axis].Axhomecfg.homespeedslow,GSS.axis[ExJogStepData.Axis].Axhomecfg.homespeedoffset,
					0,0);	
        TimerRst(&ExJogStepData.tm);
        ExJogStepData.count++;
        ExJogStepData.step = 2;
        break;

    case 2://每隔1MS发送一次速度 发送三次
        if(TimerCnt(&ExJogStepData.tm,1) > 1)
        {
            if(ExJogStepData.count > 2)
            {
                ExJogStepData.step = 3;
            }
            else
            {
                ExJogStepData.step = 1;
            }
        }
        break;

    case 3://每隔1MS发送一次速度 发送三次
        HZ_ExAxMoveRel(ExJogStepData.Axis - PULS_NUM, ExJogStepData.pos);
        ExJogStepData.step = 4;
        break;

    case 4://结束
        Data.ExSendFlag = 0;
        PARAINIT(ExJogStepData);
        break;
	
	default:
		PARAINIT(ExJogStepData);
		break;
    }
}
