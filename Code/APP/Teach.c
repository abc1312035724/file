#include "Teach.h"
#include "math.h"

void Teach()
{
    if(GUR.RunStatus==RUN)
    {
		GUW.GotoPos = 0;
		GUW.TeachWord = 0;
        return;
    }
	switch(GUW.TeachWord)
	{
		case 1://料盘预备位
			GPOS.FlowReady = GSR.AxisUnitPosition[AxFlow];
			break;
		
		case 2://料盘定长
			GPOS.FlowFeed = GSR.AxisUnitPosition[AxFlow];
			break;
		
		case 3://主架取线位
			GPOS.PickZTake = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 4://主架放线位
			GPOS.PickZPut = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 5://拉线回退位
			GPOS.PullBack = GSR.AxisUnitPosition[AxPull];
			break;
		
		case 6://拉线取线位
			GPOS.PullTake = GSR.AxisUnitPosition[AxPull];
			break;
		
		case 7://前左取线位
			GPOS.PickTakeLF = GSR.AxisUnitPosition[AxPickLF];
			break;
		
		case 8://前左放线位
			GPOS.PickPutLF = GSR.AxisUnitPosition[AxPickLF];
			break;
		
		case 9://前右取线位
			GPOS.PickTakeRF = GSR.AxisUnitPosition[AxPickRF];
			break;
		
		case 10://前右放线位
			GPOS.PickPutRF = GSR.AxisUnitPosition[AxPickRF];
			break;
		
		case 11://后左取线位
			GPOS.PickTakeLB = GSR.AxisUnitPosition[AxPickLB];
			break;
		
		case 12://后左放线位
			GPOS.PickPutLB = GSR.AxisUnitPosition[AxPickLB];
			break;
		
		case 13://后右取线位
			GPOS.PickTakeRB = GSR.AxisUnitPosition[AxPickRB];
			break;
		
		case 14://后右放线位
			GPOS.PickPutRB = GSR.AxisUnitPosition[AxPickRB];
			break;
		
		case 15://主架预备位
			GPOS.PickZReady = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 16://焊接上位
//			GPOS.WeldReady = GSR.AxisUnitPosition[AxWeld];
			break;
		
		case 17://焊接下位
//			GPOS.WeldWork = GSR.AxisUnitPosition[AxWeld];
			break;
		
		case 18://打料上
//			GPOS.UnloadUp = GSR.AxisUnitPosition[AxHit];
			break;
		
		case 19://打料下
//			GPOS.UnloadDown = GSR.AxisUnitPosition[AxHit];
			break;
			
//		case 20://拉线回退位
//			GPOS.PullBack_2 = GSR.AxisUnitPosition[AxPull_2];
//			break;
//		
//		case 21://拉线取线位
//			GPOS.PullTake_2 = GSR.AxisUnitPosition[AxPull_2];
//			break;
	}
	if(GUW.TeachWord)
	{
		GUR.HaveToReset = 1;
	}
    GUW.TeachWord = 0;
	
	switch(GUW.GotoPos)
	{
		case 1://料盘预备位
			AxAbsUnit(AxFlow,10,GPOS.FlowReady);
			break;
		
		case 2://料盘定长
			HZ_AxSetCurPos(AxFlow,GPOS.FlowReady - GPOS.FlowFeed);
			AxRelUnit(AxFlow,10,GPOS.FlowFeed);
			break;
		
		case 3://主架取线位
			AxAbsUnit(AxPickZ,10,GPOS.PickZTake);
			break;
		
		case 4://主架放线位
			AxAbsUnit(AxPickZ,10,GPOS.PickZPut);
			break;
		
		case 5://拉线回退位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPull,10,GPOS.PullBack);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 6://拉线取线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPull,10,GPOS.PullTake);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 7://前左取线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLF,10,GPOS.PickTakeLF);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 8://前左放线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLF,10,GPOS.PickPutLF);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 9://前右取线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRF,10,GPOS.PickTakeRF);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 10://前右放线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRF,10,GPOS.PickPutRF);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 11://后左取线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLB,10,GPOS.PickTakeLB);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 12://后左放线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLB,10,GPOS.PickPutLB);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 13://后右取线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRB,10,GPOS.PickTakeRB);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 14://后右放线位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRB,10,GPOS.PickPutRB);
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 15://主架预备位
			AxAbsUnit(AxPickZ,10,GPOS.PickZReady);
			break;
		
		case 16://焊接上位
//			AxAbsUnit(AxWeld,10,GPOS.WeldReady);
			break;
		
		case 17://焊接下位
//			AxAbsUnit(AxWeld,10,GPOS.WeldWork);
			break;
				
		case 18://打料上
//			AxAbsUnit(AxHit,10,GPOS.UnloadUp);
			break;
		
		case 19://打料下
//			AxAbsUnit(AxHit,10,GPOS.UnloadDown);
			break;
			
//		case 20://拉线回退位
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				AxAbsUnit(AxPull_2,10,GPOS.PullBack_2);
//			else
//				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
//			break;
//		
//		case 21://拉线取线位
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				AxAbsUnit(AxPull_2,10,GPOS.PullTake_2);
//			else
//				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
//			break;
		
		case 100://位移回零+预备位
			LT.AxGohomeReady[AxFlow].execute = 1;
			break;
		
		case 101://主架回零+预备位
			LT.AxGohomeReady[AxPickZ].execute = 1;
			break;
		
		case 102://拉线回零+预备位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPull].execute = 1;
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 103://左前回零+预备位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickLF].execute = 1;
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 104://右前回零+预备位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickRF].execute = 1;
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 105://左后回零+预备位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickLB].execute = 1;
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 106://右后回零+预备位
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickRB].execute = 1;
			else
				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
			break;
		
		case 107://焊线回零+预备位
//			LT.AxGohomeReady[AxWeld].execute = 1;
			break;
		
		case 108://打料轴回零+预备位
//			LT.AxGohomeReady[AxHit].execute = 1;
			break;
			
//		case 109://拉线2回零+预备位
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				LT.AxGohomeReady[AxPull_2].execute = 1;
//			else
//				AlarmSetBit(2,0,7);//请将主架回到预备位再进行其他操作
//			break;
		
	}
	if(GUW.GotoPos)
	{
		GUR.HaveToReset = 1;
	}
	GUW.GotoPos = 0;
}

