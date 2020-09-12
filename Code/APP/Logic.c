 /*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 8轴底层
 * Description:用户逻辑
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"
#include "math.h"

#define PI 3.1415926


/*全局变量做统一处理*/
Task LogicTask = { 0 }; //统一的任务类
SysData Data = { 0 };  //统一的数据类
Gpio I_BeltSupply[2] = {I_BeltSupplyL,I_BeltSupplyR};
Gpio Q_BeltSupply[2] = {Q_BeltSupplyL,Q_BeltSupplyR};
//u32	AxBelt[2] = {AxBelt_1,AxBelt_2};



#define	AxPickPPR	2000


//设备主流程
UphDef DeveceUph;
void DeviceMain(LogicParaDef *p)
{
	static u32 Tm_I_EntranceInp,I_EntranceInp_Ok;
	//外部口罩提供设备的启动信号控制
	if(FSM_STA == RUN)//机台运行状态
	{			
		if(GCFG.EntranceInpEn == 0)
		{
			Q_Set(Q_MaskSupply,OFF);
		}
		else
		{
			Q_Set(Q_MaskSupply,ON);//控制外部口罩机供料
		}
		Q_Set(Q_ExitMotor,ON);
		my.execute = 1;
		LT.SectionInpTask.execute = 1;
	}
	else
	{
		PARAINIT(LT.SectionInpTask);
	}
	//一直检测入口状态，并完成滤波，不要等待流程开始跑的时候再检测，减少时间
	if(TimerCnt(&Tm_I_EntranceInp,I_Get(I_EntranceInp) == ON) > GDLY.EntranceInpDelay)
	{
		I_EntranceInp_Ok = 1;
	}
	else
	{
		I_EntranceInp_Ok = 0;
	}
	if(FSM_STA != RUN)return;
	TASK_PROCESS
	{
		case 1://等待入口有口罩，位移轴移动一格
			if((I_EntranceInp_Ok == 1 || GCFG.EntranceInpEn == 0))
			{
				AxRelUnit(AxFlow,GSPD,GPOS.FlowFeed);
				my.step = 2;
			}
			break;
			
		case 2://位移到位后，执行下料和焊接动作
			if(AxSta(AxFlow) == AXSTA_READY)
			{
				HZ_AxSetCurPos(AxFlow,GPOS.FlowReady);  
				LT.WeldTask.execute = 1;
				my.step = 3;
			}
			break;
			
		case 3://焊接完成后，计算产能，结束
			if(LT.WeldTask.execute == 0)
			{
				UphCal(&DeveceUph);
				GUS.Product.Data.Pcs_Min = DeveceUph.Upm;
				
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//取线，取拉好的线，到焊位上方等待
void Pick(LogicParaDef *p)
{
	static u32 Tm,Tm2;
	TASK_PROCESS
	{
		case 1:
			Q_Set(Q_PickClamp,OFFR);//张开夹子
			TimerRst(&Tm);//延时
			my.step = 2;
			break;
			
		case 2://升降轴到预备位
			if(AxSta(AxPickZ) == AXSTA_READY && TimerCnt(&Tm,1) >= GDLY.PickClampOffDelay)
			{
			//	if(fabs(GSR.AxisUnitPosition[AxPickZ] - GPOS.PickZReady) < 50)
				{
					AxAbsUnit(AxPickZ,GSPD,GPOS.PickZReady);
				}
				//else
				//{
				//	AxRelUnit(AxPickZ,GSPD,AxPickPPR - GPOS.PickZPut + GPOS.PickZReady);
				//}
				my.step = 3;
			}
			break;
		
		case 3://升降轴到位，前后左右到取线位
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY)
			{
//				HZ_AxSetCurPos(AxPickZ,GPOS.PickZReady);
				AxAbsUnit(AxPickLB,GSPD,GPOS.PickTakeLB);
				AxAbsUnit(AxPickLF,GSPD,GPOS.PickTakeLF);
				AxAbsUnit(AxPickRB,GSPD,GPOS.PickTakeRB);
				AxAbsUnit(AxPickRF,GSPD,GPOS.PickTakeRF);
				my.step = 4;
			}
			break;
			
		case 4://前后左右到位
			if(AxInpSta(AxPickLB) == AXSTA_READY &&
			   AxInpSta(AxPickLF) == AXSTA_READY &&
			   AxInpSta(AxPickRB) == AXSTA_READY &&
			   AxInpSta(AxPickRF) == AXSTA_READY)
			{
				//如果拉线没完成，拉一次线
				if(LT.PullTask.done != 1)
				{
					LT.PullTask.execute = 1;
				}
				my.step = 5;
			}
			break;
			
		case 5:
			if(LT.PullTask.execute == 0)//拉线结束
			{
				if(LT.PullTask.done == 1)//判断拉线结果，OK继续
				{
					LT.PullTask.done = 0;
					my.step = 6;
				}
				else//NG结束，拉线失败，跳过这个
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 6://升降轴到取线位
			if(AxSta(AxPickZ) == AXSTA_READY)
			{
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZTake);
				my.step = 7;
			}
			break;
			
		case 7://升降轴到位
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY)
			{
				TimerRst(&Tm);
				Q_Set(Q_PickClamp,ONR);//取线夹夹紧
				my.step = 8;
			}
			break;
			
		case 8://延时后
			if(TimerCnt(&Tm,1) >= GDLY.PickClampOnDelay)
			{
				Q_Set(Q_PullClamp,OFFR);//拉线夹子张开
				Q_Set(Q_Cut,ON);//剪刀剪线
				TimerRst(&Tm);
				TimerRst(&Tm2);
				my.step = 9;
			}
			break;
			
		case 9://延时后
			if(TimerCnt(&Tm,1) >= GDLY.CutOnDelay)
			{
				Q_Set(Q_Cut,OFF);//剪刀张开
				TimerRst(&Tm);
				my.step = 10;
			}
			break;
			
		case 10://延时后
			if(TimerCnt(&Tm2,1) >= GDLY.PullClampOffDelay && TimerCnt(&Tm,1) >= GDLY.CutOffDelay)
			{
//				AxRelUnit(AxPickZ,GSPD,AxPickPPR - GPOS.PickZTake + GPOS.PickZReady);//升降轴到上位置
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZReady);
				my.step = 11;
			}
			break;
			
		case 11://升降到上位置后
			if(AxSta(AxPickZ) == AXSTA_READY)
			{
				LT.PullTask.execute = 1;//拉线
				my.step = 12;
			}
			break;
			
		case 12://轴都在预备状态，且点焊气缸在上位置
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY) 
				//&& AxSta(AxWeld) == AXSTA_READY)
			{
				//前后左右到放线位
//				HZ_AxSetCurPos(AxPickZ,GPOS.PickZReady);
				AxAbsUnit(AxPickLB,GSPD,GPOS.PickPutLB);
				AxAbsUnit(AxPickLF,GSPD,GPOS.PickPutLF);
				AxAbsUnit(AxPickRB,GSPD,GPOS.PickPutRB);
				AxAbsUnit(AxPickRF,GSPD,GPOS.PickPutRF);
				my.step = 13;
			}
			break;
			
		case 13://前后左右到位后，结束
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxInpSta(AxPickLB) == AXSTA_READY &&
			   AxInpSta(AxPickLF) == AXSTA_READY &&
			   AxInpSta(AxPickRB) == AXSTA_READY &&
			   AxInpSta(AxPickRF) == AXSTA_READY)
			{
				my.step = 0;
				my.execute = 0;
				my.done = 1;
			}
			break;
	}
}

//焊线
void Weld(LogicParaDef *p)
{
	static u32 Tm;
	static u32 Tm_WeldUp,Tm_WeldDown;
	//点焊气缸报警
	if(TimerCnt(&Tm_WeldDown,Q_Get(Q_WeldLift) == ON && I_Get(I_WeldCydDown) != ON) > 800)
	{
		AlarmSetBit(2,0,4);//点焊气缸下感应异常
		PARAINIT(LT.WeldTask);
	}
	if(TimerCnt(&Tm_WeldUp,Q_Get(Q_WeldLift) == OFF && I_Get(I_WeldCydUp) != ON) > 800)
	{		
		AlarmSetBit(2,0,5);//点焊气缸上感应异常
		PARAINIT(LT.WeldTask);
	}
	
	TASK_PROCESS
	{
		case 1://延时
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://焊前检测有料
			if(my.count || GCFG.EntranceInpEn == 0)
			{
				if(my.count > 0)//my.count用来作为焊前检测到料的标志
				{
					my.count--;
				}
				my.step = 3;//去焊接
			}
			else
			{
				my.step = 10;//去结束
			}
			break;
		
		case 3://查询是否取好线了
			if(LT.PickTask.done != 1)
			{
				LT.PickTask.execute = 1;//没有就去取
			}
			my.step = 4;
			break;
			
		case 4://确保取线完成
			if(LT.PickTask.execute == 0 ) //&& AxSta(AxWeld) == AXSTA_READY)
			{
				if(LT.PickTask.done == 1)
				{
					LT.PickTask.done = 0;
					AxAbsUnit(AxPickZ,100,GPOS.PickZPut);//升降轴下降
					Q_Set(Q_WeldLift,ON);
					//AxAbsUnit(AxWeld,GSPD,GPOS.WeldWork);//点焊气缸下降
					if(GCFG.WeldEn == 1)
					{
						Q_Set(Q_WeldLeft,ON);//超声波开
						Q_Set(Q_WeldRight,ON);//超声波开
					}
					TimerRst(&Tm);
					my.step = 5;
				}
				else//取线失败，结束，跳过这个
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 5://等待升降轴和点焊气缸下到位
			if(AxSta(AxPickZ) == AXSTA_READY && (I_Get(I_WeldCydDown)==ON || AlarmGetBit(2,0,4)))//AxSta(AxWeld) == AXSTA_READY)
			{
				LT.PickTask.execute = 1;//取线又可以去取线了
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://等待超声波延时结束
			if(TimerCnt(&Tm,1) >= GDLY.WeldOnDelay)
			{
				Q_Set(Q_WeldLeft,OFF);//关闭超声波
				Q_Set(Q_WeldRight,OFF);//关闭超声波
				TimerRst(&Tm);
				my.step = 7;
			}
			break;
			
		case 7://等待固话时间
			if(TimerCnt(&Tm,1) >= GDLY.WeldOffDelay)
			{
				//AxAbsUnit(AxWeld,GSPD,GPOS.WeldReady);
				Q_Set(Q_WeldLift,OFF);
				TimerRst(&Tm);
				my.step = 8;
			}
			break;
			
		case 8 :
			if(I_Get(I_WeldCydDown) == OFF)
			{
				TimerRst(&Tm);
				my.step = 9;
			}
			
			break;
		
		case 9://等待轴结束，或者当前位置离预备位小于行程的1/3时
			if( (I_Get(I_WeldCydUp) == ON  || (TimerCnt(&Tm,1) > GDLY.WeldLiftOffDelay)) && FSM_STA == RUN)
			{
				my.step = 10;//去结束
			}
			break;
			
		case 10:
			if(I_Get(I_WeldInp) == ON)//检测一下焊前来料检测
			{
				my.count++;//有料就++
			}
			my.execute = 0;
			my.step = 0;
			break;
	}
}

//拉线
void Pull(LogicParaDef *p)
{
	static u32 Tm,Tm2;
	static u32 I_BeltSupply_Off[2];
	//检测送线感应在拉线过程中有没有OFF信号，有就代表拉到线了
	for(int i=0; i<2; i++)
	{
		if(I_Get(I_BeltSupply[i]) == OFF)
		{
			I_BeltSupply_Off[i] = 1;
			//LT.TrembleTask.execute = 1;
		}

	}
	TASK_PROCESS
	{
		case 1://拉线夹子张开，拉线轴到取线位
			I_BeltSupply_Off[0] = 0;
			I_BeltSupply_Off[1] = 0;
			Q_Set(Q_PullClamp,OFFR);
			if(GUR.StartSlowCnt)
			{
				AxAbsUnit(AxPull,30,GPOS.PullTake);
			}
			else
			{
				AxAbsUnit(AxPull,GSPD,GPOS.PullTake);
			}
			TimerRst(&Tm2);
			my.step = 2;
			break;
		
		case 2://夹子夹紧
			if(AxSta(AxPull) == AXSTA_READY  && TimerCnt(&Tm2,1) >= GDLY.DelayPull)
			{
				Q_Set(Q_PullClamp,ONR);
				TimerRst(&Tm);
				TimerRst(&Tm2);
				my.step = 3;
			}
			break;
			
		case 3://压线松开
			if(TimerCnt(&Tm,1) >= GDLY.PullClampOnDelay)
			{
				Q_Set(Q_Press,ON);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://拉线轴到拉线后位
			if(TimerCnt(&Tm,1) >= GDLY.PressOnDelay)
			{
				TimerRst(&Tm);
				if(GUR.StartSlowCnt)
				{
					AxAbsUnit(AxPull,30,GPOS.PullBack);
				}
				else
				{
					AxAbsUnit(AxPull,GSPD,GPOS.PullBack);
				}
				my.step = 5;
			}
			break;
			
		case 5://拉了一段时间之后，或者拉线轴结束了，就压线
			if(AxSta(AxPull) == AXSTA_READY )
			{
				//Q_Set(Q_Press,OFF);
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://拉线电机走到拉线后位置后延时压带
			if(TimerCnt(&Tm,1) >= GDLY.DelayPress)
			{
				Q_Set(Q_Press,OFF);
				TimerRst(&Tm);
				my.step = 7;
			}
			break;
			
		case 7://压线气缸动作时间 且 拉线轴结束，流程结束
			if(TimerCnt(&Tm,1) >= GDLY.PressOffDelay && AxSta(AxPull) == AXSTA_READY)
			{
				LT.BeltSupplyTask[0].execute = 1;
				LT.BeltSupplyTask[1].execute = 1;
				my.execute = 0;
				my.step = 0;
				if(GCFG.PullAlmEn == 1 && (I_BeltSupply_Off[0] == 0 || I_BeltSupply_Off[1] == 0))
				{
					my.count ++;
					if(my.count >= 3)
					{
						AlarmSetBit(2,0,6);//拉耳线动作失败
						my.count = 0;
					}
				}
				else
				{	
					my.count = 0;
					my.done = 1;
				}
				if(GUR.StartSlowCnt)
				{
					GUR.StartSlowCnt--;
				}
			}
			break;
	}
}

//供线
void BeltSupply(LogicParaDef *p, s32 i)
{
	static u32 Tm[2];
	//当浮棍下信号没有感应时
	if(I_Get(I_BeltSupply[i]) == OFF)
	{
		my.execute = 1;
		if(GCFG.TrembleEn == 1)
			  LT.TrembleTask.execute = 1;
	}
	TASK_PROCESS
	{		
		case 1://每次拉线后会执行一次供线，如果拉线后供线信号没有感应
			if(I_Get(I_BeltSupply[i]) == ON)
			{
				my.execute = 0;
				my.step = 0;
			}
			else
			{
				TimerRst(&Tm[i]);
				Q_Set(Q_BeltSupply[i],ON);//供线电机开
				//AxRelUnit(AxBelt[i],100,999999);//供线步进开
				my.count = 0;
				my.step = 2;
			}
			break;
			
		case 2://供线下感应有信号
			if(I_Get(I_BeltSupply[i]) == ON)
			{
				Q_Set(Q_BeltSupply[i],OFF);//关闭供线电机
				//AxStop(AxBelt[i]);//关闭供线步进
//				HZ_AxSetCurPos(AxBelt[i],0);
				my.execute = 0;
				my.step = 0;
			}
//			else if(TimerCnt(&Tm[i],1) >= 5000 && GCFG.PullAlmEn == 1)//如果供线电机运行超过供线时间内
//			{
//				AxStop(AxBelt[i]);
//				Q_Set(Q_BeltSupply[i],OFF);//关闭供线电机
//				AlarmSetBit(2,0,2+i);//耳带卡或送带电机太慢
//				my.execute = 0;
//				my.step = 0;
//			}
			break;
	}
}

//出料打料
void UnloadHit(LogicParaDef *p)
{
	static u32 Tm,I_ExitCheck_Tm,I_ExitCheck_val;
	static TrigDef Tr;
	if(TimerCnt(&I_ExitCheck_Tm,I_Get(I_ExitCheck) == ON) > 100)
	{
		I_ExitCheck_val = ON;
	}
	else
	{
		I_ExitCheck_val = OFF;
	}
	if(TrigOne(&Tr,I_ExitCheck_val == ON && (FSM_STA == RUN || FSM_STA == D_RESET) && GUR.HitReady == 1, 20))//检测口罩通过下料感应
	{
		GUS.Product.Data.Cnt++;
		my.execute = 1;//执行一次打料
	}
	TASK_PROCESS
	{
		case 1:
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://气缸下，延时
			if(TimerCnt(&Tm,1) >= GDLY.UnloadInp)
			{
				//AxRelUnit(AxHit,100,GPOS.UnloadDown);
				//AxAbsUnit(AxHit,100,GPOS.UnloadDown);
				Q_Set(Q_UnloadCyd,ON);
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
		
		case 3:////延时完了气缸上
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOnDelay ) //&& AxSta(AxHit) == AXSTA_READY)
			{
				//AxAbsUnit(AxHit,100,GPOS.UnloadUp);
				Q_Set(Q_UnloadCyd,OFF);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://延时后结束，技术够了执行一次皮带
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOffDelay)// && AxSta(AxHit) == AXSTA_READY)
			{
				my.execute = 0;
				my.step = 0;
				my.count ++;
				if(my.count >= GCFG.UnloadPackNum)//数量大于打包数量
				{
					my.count = 0;
					if(GCFG.UnloadPackNum > 0)
					{
						LT.UnloadBeltTask.execute = 1;//执行一次皮带动
					}
				}
			}
			break;
	}
}

//出料皮带
void UnloadBelt(LogicParaDef *p)
{
	static u32 tm;
	TASK_PROCESS
	{
		case 1://延时启动
			TimerRst(&tm);
			my.step = 2;
			break;
		
		case 2://启动皮带
			if(TimerCnt(&tm,1) > GDLY.UnloadBeltDelay)
			{
				Q_Set(Q_CollectMotor,ON);
				TimerRst(&tm);
				my.step = 3;
			}
			break;
		
		case 3://延时后关闭
			if(TimerCnt(&tm,1) > GDLY.UnloadBeltTime)
			{
				Q_Set(Q_CollectMotor,OFF);
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

void SectionInp(LogicParaDef *p)
{
	static u32 tm,SectionInp_OK;
		//一直检测入口状态，并完成滤波，不要等待流程开始跑的时候再检测，减少时间
	if(TimerCnt(&tm,I_Get(I_Section) == ON) > GDLY.SectionDelay)
	{
		SectionInp_OK = 1;
	}
	else
	{
		SectionInp_OK = 0;
	}
	
	TASK_PROCESS
	{
		case 1 :
			if( GCFG.SectionDlEn == 1 && GCFG.EntranceInpEn == 1)
			{
				my.step = 2;
			}
			else
			{				
				PARAINIT(LT.SectionInpTask);
			}
			break ;
		
		case 2 :
			if((I_Get(I_EntranceInp) == ON && I_Get(I_Section) == ON) || SectionInp_OK == 1)
			{
				AlarmSetBit(2,0,8);//入口叠料报警
				PARAINIT(LT.SectionInpTask);
			}			
			break;			
	}
}


Gpio I_Cloth[3] = {I_Cloth1,I_Cloth2,I_Cloth3};
//布料纠偏
void RollCorrect()
{
	static u32 tm[3],tm1[3];
	for(int i = 0;i<3;i++)
	{
		if(TimerCnt(&tm1[i],I_Get(I_Cloth[i]) == OFF) > 50 && I_Get(I_MaskAuto) == ON && FSM_STA == RUN)
		{
			AlarmSetBit(2,0,7);//打片机缺布料
		}

//		if(LT.SlipTask.execute == 1 && GCFG.RollCorrectEn == 0)
//		{
//			if(I_Get(I_RollL[i]) == ON && I_Get(I_RollR[i]) == ON )
//			{
//					Q_Set(Q_RollPos[i],OFF);
//					Q_Set(Q_RollNeg[i],ON);
//				
//			}
//			else if(I_Get(I_RollL[i]) == OFF && I_Get(I_RollR[i]) == OFF)
//			{

//					Q_Set(Q_RollPos[i],ON);
//					Q_Set(Q_RollNeg[i],OFF);

//			}
//			else
//			{
//				Q_Set(Q_RollPos[i],OFF);
//				Q_Set(Q_RollNeg[i],OFF);
//				TimerRst(&tm[i]);
//			}
//			
//			if(I_Get(I_Cloth[i]) == OFF || GCFG.RollCorrectEn != 0)
//			{
//				Q_Set(Q_RollPos[i],OFF);
//				Q_Set(Q_RollNeg[i],OFF);					
//			}
//		}		
	}
}



//打片机逻辑
void Slip(LogicParaDef *p)
{
	static u32 tm,tm1; 
	static u32 speedTerm;
	static u32 currSpeed,targetSpeed;
	if(I_Get(I_MaskAuto) == ON)
	{
		if(FSM_STA == RUN)
		{
			my.execute = 1;
		}
		else
		{
			my.execute = 0;
		}
	}
	else 
	{
		if(I_Get(I_MaskManual) == ON)
		{
			my.execute = 1;
		}
		else
		{
			my.execute = 0;
		}
	}
	RollCorrect();
	targetSpeed = GCFG.MaskSpeed*GUS.Config.Data.GlobalSpeed/100;
	if(TimerCnt(&tm1,1) > 500)
	{
		if(HZ_AxGetStatus(AxMask) == AXSTA_DISCRETE)
		{
			if(currSpeed<targetSpeed)
			{
				currSpeed += 1;
				if(currSpeed > targetSpeed)
				{
					currSpeed = targetSpeed;
				}			
			}
			else if(currSpeed > targetSpeed)
			{
				currSpeed -= 1;
				if(currSpeed < targetSpeed)
				{
					currSpeed = targetSpeed;
				}
			}
		}
        else
		{
			currSpeed = 1;
		}			
		TimerRst(&tm1);
	}
	
	TASK_PROCESS
	{
		case 1://延时启动
			TimerRst(&tm);
			my.step = 2;
			break;
		
		case 2://启动皮带
			if(HZ_AxGetStatus(AxMask) == 0)
			{
				Q_Set(Q_MaskWeld,ON);
				Q_Set(Q_SlipUnfeedMotor,ON);
				AxRelUnit(AxMask,currSpeed,999999999);	
			    speedTerm = currSpeed;
				TimerRst(&tm);
				my.step = 3;
			}
			break;
		
		case 3://延时后关闭
			if(currSpeed != speedTerm)
			{
				speedTerm = currSpeed;
				AxSpeedReload(AxMask,currSpeed);
			}
			
			if(my.execute == 0)
			{
				Q_Set(Q_MaskWeld,OFF);
				Q_Set(Q_SlipUnfeedMotor,OFF);
//				for(int i=0;i<3;i++)
//				{
//					Q_Set(Q_RollPos[i],OFF);
//					Q_Set(Q_RollNeg[i],OFF);	
//				}
				HZ_AxStopDec(AxMask);
				my.execute = 0;
				my.step = 0;
			}
			break;
	}	
}


void ResetToCut(LogicParaDef *p)
{
	static u32 Tm;
	TASK_PROCESS
	{
		case 1:
			Q_Set(Q_Cut,ON);//剪刀剪线
			TimerRst(&Tm);
			my.step = 2;
			break;
			
		case 2://剪线时间
			if(TimerCnt(&Tm,1) >= GDLY.CutOnDelay)
			{
				Q_Set(Q_PullClamp,OFFR);//拉线夹子张开
				Q_Set(Q_Cut,OFF);//剪刀张开
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
			
		case 3://剪刀开延时
			if(TimerCnt(&Tm,1) >= GDLY.CutOffDelay)
			{
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

void ManualDisBelt(LogicParaDef *p)
{	
	static u32 Tm;
	TASK_PROCESS
	{
		case 1://张开夹子
			Q_Set(Q_PullClamp,OFFR);
			Q_Set(Q_PickClamp,OFFR);
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://夹子夹紧
			if(TimerCnt(&Tm,1) >= 60)
			{
				Q_Set(Q_PullClamp,ONR);
				Q_Set(Q_PickClamp,ONR);
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}
//抖线气缸
void Tremble(LogicParaDef *p)
{
	static u32 Tm;
	TASK_PROCESS
	{
		case 1:
			Q_Set(Q_TrembleClamp,OFFR);
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2:
			if(TimerCnt(&Tm,1) >= 200)
			{
				Q_Set(Q_TrembleClamp,ONR);
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
		case 3:
			if(TimerCnt(&Tm,1) >= 200)
			{
				Q_Set(Q_TrembleClamp,OFFR);
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//轴回零+预备位
void AxGohomeReady(LogicParaDef *p,u32 i)
{
	TASK_PROCESS
	{
		case 1:
			AxGohome(i);
			my.step = 2;
			break;
		
		case 2:
			if(AxSta(i) == AXSTA_READY)
			{
				switch(i)
				{
					case AxFlow:
						AxAbsUnit(i,10,GPOS.FlowReady);
						break;
					
					case AxPickZ:
						AxAbsUnit(i,10,GPOS.PickZReady);
						break;
					
					case AxPull:
						AxAbsUnit(i,10,GPOS.PullBack);
						break;
					
//					case AxWeld:
//						AxAbsUnit(i,10,GPOS.WeldReady);
//						break;
					
					case AxPickLF:
						AxAbsUnit(i,10,GPOS.PickTakeLF);
						break;
					
					case AxPickRF:
						AxAbsUnit(i,10,GPOS.PickTakeRF);
						break;
					
					case AxPickLB:
						AxAbsUnit(i,10,GPOS.PickTakeLB);
						break;
					
					case AxPickRB:
						AxAbsUnit(i,10,GPOS.PickTakeRB);
						break;
					
//					case AxHit:
//						AxAbsUnit(i,10,GPOS.UnloadUp);
//						break;
//					case AxPull_2:
//						AxAbsUnit(i,10,GPOS.PullBack_2 );
//						break;
				}
				my.step = 3;
			}
			break;
			
		case 3:
			if(AxSta(i) == AXSTA_READY)
			{
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//本应用的轴配置
void AxisDataConfig()
{
	GSS.axis[0].Axhomecfg.orglev = ON;
	GSS.axis[1].Axhomecfg.orglev = ON;
	GSS.axis[2].Axhomecfg.orglev = ON;
	GSS.axis[3].Axhomecfg.orglev = ON;
	GSS.axis[4].Axhomecfg.orglev = ON;
	GSS.axis[5].Axhomecfg.orglev = ON;
	GSS.axis[6].Axhomecfg.orglev = ON;
	GSS.axis[7].Axhomecfg.orglev = ON;
	GSS.axis[8].Axhomecfg.orglev = ON;
	GSS.axis[9].Axhomecfg.orglev = ON;
	GSS.axis[10].Axhomecfg.orglev = ON;
	GSS.axis[11].Axhomecfg.orglev = ON;
	
//	GUS.AxisCfg.Data.AccTime[AxBelt_2] = GUS.AxisCfg.Data.AccTime[AxBelt_1];
//	GUS.AxisCfg.Data.Direction[AxBelt_2] = GUS.AxisCfg.Data.Direction[AxBelt_1];
//	GUS.AxisCfg.Data.HomeSpd[AxBelt_2] = GUS.AxisCfg.Data.HomeSpd[AxBelt_1];
//	GUS.AxisCfg.Data.RunSpd[AxBelt_2] = GUS.AxisCfg.Data.RunSpd[AxBelt_1];
//	GUS.AxisCfg.Data.StartSpd[AxBelt_2] = GUS.AxisCfg.Data.StartSpd[AxBelt_1];

//	GUS.AxisCfg.Data.AccTime[AxPull_2] = GUS.AxisCfg.Data.AccTime[AxPull];
////	GUS.AxisCfg.Data.Direction[AxPull_2] = GUS.AxisCfg.Data.Direction[AxPull];
//	GUS.AxisCfg.Data.HomeSpd[AxPull_2] = GUS.AxisCfg.Data.HomeSpd[AxPull];
//	GUS.AxisCfg.Data.RunSpd[AxPull_2] = GUS.AxisCfg.Data.RunSpd[AxPull];
//	GUS.AxisCfg.Data.StartSpd[AxPull_2] = GUS.AxisCfg.Data.StartSpd[AxPull];
//	if(GUS.AxisCfg.Data.Direction[AxPull] == 0)
//		GUS.AxisCfg.Data.Direction[AxPull_2] = 1;
//	else
//		GUS.AxisCfg.Data.Direction[AxPull_2] =0;

	for(int i=0;i<PULS_NUM;i++)
	{
		GSS.axis[i].Axconver.MPR = 1.0f;
		GSS.axis[i].Axconver.PPR = 1;
		GSS.axis[i].Axhomecfg.homemode = 0;
		GSS.axis[i].Axhomecfg.homespeedoffset = 0;
		GSS.axis[i].Axhomecfg.homespeedslow = 1000;
		GSS.axis[i].Axhomecfg.orgnum = i;
        GSS.axis[i].Axlimitcfg.alarmmode = 2;
        GSS.axis[i].Axlimitcfg.limitMode = 0;
		GSS.axis[i].AxSpd.acctime = GUS.AxisCfg.Data.AccTime[i];
        GSS.axis[i].AxSpd.dectime = GUS.AxisCfg.Data.AccTime[i];
		GSS.axis[i].Axhomecfg.homespeedfast = GUS.AxisCfg.Data.HomeSpd[i];
		GSS.axis[i].AxSpd.startspeed = GUS.AxisCfg.Data.StartSpd[i];
        GSS.axis[i].AxSpd.endspeed = GUS.AxisCfg.Data.StartSpd[i];
		
		if(GSW.JogForward[0] == 0 && GSW.JogBackward[0] == 0)
		{
			GSS.axis[i].AxSpd.runspeed = GUS.AxisCfg.Data.RunSpd[i];
		}
		else
		{
			GSS.axis[AxFlow].AxSpd.runspeed = 500;
			GSS.axis[AxPickZ].AxSpd.runspeed = 500;
			GSS.axis[AxPull].AxSpd.runspeed = 500;
			GSS.axis[AxPickLB].AxSpd.runspeed = 200;
			GSS.axis[AxPickLF].AxSpd.runspeed = 200;
			GSS.axis[AxPickRB].AxSpd.runspeed = 200;
			GSS.axis[AxPickRF].AxSpd.runspeed = 200;
//			GSS.axis[AxWeld].AxSpd.runspeed = 500;
//			GSS.axis[AxHit].AxSpd.runspeed = 500;
//			GSS.axis[AxBelt_1].AxSpd.runspeed = 500;
//		    GSS.axis[AxBelt_2].AxSpd.runspeed = 500;
//			GSS.axis[AxPull_2].AxSpd.runspeed = 500;
		}
		if(i == AxFlow)
		{
			GSS.axis[i].Axhomecfg.homespeedslow = GSS.axis[i].Axhomecfg.homespeedfast;
		}
//		if(i == AxHit)
//		{
//			GSS.axis[i].Axhomecfg.orgnum = 20;
//		}
//		if(i == AxPull_2)
//		{
//			GSS.axis[i].Axhomecfg.orgnum = 20;
//		}
		AxDirCfg(i,GUS.AxisCfg.Data.Direction[i]);
		AxisHomeCfg(i,GSS.axis[i].Axhomecfg);
	}
}

//本设备的按钮操作
void DeviceBtn()
{
	static TrigDef Tr_Start,Tr_Stop,Tr_Scram,Tr_Reset,Tr_Manual,Tr_Press;
	static u32 Tm_Scram;
	//复位前，按钮执行复位，复位后，按钮运行
	if(TrigOne(&Tr_Start,I_Get(I_Start) == ON,50))
	{
//		if(GUR.HaveToReset == 1)
//		{
//			FSM_CMD = D_RESET;
//		}
//		else
		{
			FSM_CMD = RUN;
		}
	}
	//复位前，按钮执行复位，复位后，按钮停止
	if(TrigOne(&Tr_Stop,I_Get(I_Stop) == ON,50))
	{
		if(GSR.ErrorLevel)
		{
			GSW.ClearAlarm = 1;
		}
		else
		{
			FSM_CMD = STOP;
		}
	}
	//复位按钮
	if(TrigOne(&Tr_Reset,I_Get(I_Reset) == ON,50))
	{
		FSM_CMD = D_RESET;
	}
	//启停弹起
	if(TrigOne(&Tr_Scram,I_Get(I_Scram) == ON,50))
	{
		FSM_CMD = STOP;
	}
	//急停拍下
	if(TimerCnt(&Tm_Scram,I_Get(I_Scram) == OFF) > 50)
	{
		GUR.HaveToReset = 1;
		for(int i=0; i<PULS_NUM; i++)
		{
			AxStop(i);
		}
		FSM_CMD = SCRAM;
	}
	
	
	//旋钮切到手动，停止
	if(TrigOne(&Tr_Manual,I_Get(I_Manual) == OFF,50))
	{
		if(FSM_STA == RUN)
		{
			FSM_CMD = STOP;
		}
	}
	//压耳按钮
	if(TrigOne(&Tr_Press,I_Get(I_PressManual) == ON,50))
	{
		if(Q_Get(Q_Press) == ON)
		{
			Q_Set(Q_Press,OFF);
		}
		else
		{
			Q_Set(Q_Press,ON);
		}
	}	
}



//本设备点动配置
void DeviceJogSet()
{
	GSW.jogSpd = 100;
	if(GUW.JogFeed == 0)
	{
		GSW.JogMode = 2;
	}
	else
	{
		GSW.JogMode = 1;
		switch(GUW.JogFeed)
		{
			case 1:
				GSW.JogPosition = 1;
				break;
			
			case 2:
				GSW.JogPosition = 5;
				break;
			
			case 3:
				GSW.JogPosition = 10;
				break;
			
			case 4:
				GSW.JogPosition = 50;
				break;
		}
	}
}
//急停后执行一次
void scram_handle()
{
	u32 i;
	GUR.HaveToReset = 1;
//	Q_Set(Q_MaskSupply,OFF);
//	Q_Set(Q_ExitMotor,OFF);
	PARAINIT(LT);
	for(i=0; i<PULS_NUM; i++)
	{
		AxStop(i);
	}
	for (i = 0; i < GPO_NUM; i++) //初始化 输出口
	{
        OutPut_SetSta(i, OFF);
	}
}

//启动后执行一次
void start_handle()
{
	PARAINIT(DeveceUph);
}
//停止后执行一次
void stop_handle()
{
	Q_Set(Q_MaskSupply,OFF);
	Q_Set(Q_ExitMotor,OFF);
}

//2020.7.20
//设备逻辑循环
void Logic()
{
	AxisDataConfig();				//本项目用的轴配置方式
	DeviceJogSet();					//本项目点动设置
	DeviceBtn();					//本设备的按钮操作
	
    /******工程运行逻辑*******/
	DeviceMain(&LT.DeviceMainTask);			//设备主流程
	Pick(&LT.PickTask);						//取线，取拉好的线，到焊位上方等待
	Weld(&LT.WeldTask);						//焊线
	Pull(&LT.PullTask);						//拉线
	BeltSupply(&LT.BeltSupplyTask[0],0);	//供线左
	BeltSupply(&LT.BeltSupplyTask[1],1);	//供线右
	UnloadHit(&LT.UnloadHitTask);			//出料打料
	UnloadBelt(&LT.UnloadBeltTask);			//出料皮带
	SectionInp(&LT.SectionInpTask);			//切片叠料感应
	ResetToCut(&LT.ResetToCutTask);			//复位剪线
	ManualDisBelt(&LT.ManualDisBeltTask);	//复位丢线
	Tremble(&LT.TrembleTask);				//抖线
	/*******打片逻辑********/
	Slip(&LT.SlipTask);	
	
	for(int i=0; i<PULS_NUM; i++)
	{
		AxGohomeReady(&LT.AxGohomeReady[i],i);
	}
}

