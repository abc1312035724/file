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
		case 1://����Ԥ��λ
			GPOS.FlowReady = GSR.AxisUnitPosition[AxFlow];
			break;
		
		case 2://���̶���
			GPOS.FlowFeed = GSR.AxisUnitPosition[AxFlow];
			break;
		
		case 3://����ȡ��λ
			GPOS.PickZTake = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 4://���ܷ���λ
			GPOS.PickZPut = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 5://���߻���λ
			GPOS.PullBack = GSR.AxisUnitPosition[AxPull];
			break;
		
		case 6://����ȡ��λ
			GPOS.PullTake = GSR.AxisUnitPosition[AxPull];
			break;
		
		case 7://ǰ��ȡ��λ
			GPOS.PickTakeLF = GSR.AxisUnitPosition[AxPickLF];
			break;
		
		case 8://ǰ�����λ
			GPOS.PickPutLF = GSR.AxisUnitPosition[AxPickLF];
			break;
		
		case 9://ǰ��ȡ��λ
			GPOS.PickTakeRF = GSR.AxisUnitPosition[AxPickRF];
			break;
		
		case 10://ǰ�ҷ���λ
			GPOS.PickPutRF = GSR.AxisUnitPosition[AxPickRF];
			break;
		
		case 11://����ȡ��λ
			GPOS.PickTakeLB = GSR.AxisUnitPosition[AxPickLB];
			break;
		
		case 12://�������λ
			GPOS.PickPutLB = GSR.AxisUnitPosition[AxPickLB];
			break;
		
		case 13://����ȡ��λ
			GPOS.PickTakeRB = GSR.AxisUnitPosition[AxPickRB];
			break;
		
		case 14://���ҷ���λ
			GPOS.PickPutRB = GSR.AxisUnitPosition[AxPickRB];
			break;
		
		case 15://����Ԥ��λ
			GPOS.PickZReady = GSR.AxisUnitPosition[AxPickZ];
			break;
		
		case 16://������λ
//			GPOS.WeldReady = GSR.AxisUnitPosition[AxWeld];
			break;
		
		case 17://������λ
//			GPOS.WeldWork = GSR.AxisUnitPosition[AxWeld];
			break;
		
		case 18://������
//			GPOS.UnloadUp = GSR.AxisUnitPosition[AxHit];
			break;
		
		case 19://������
//			GPOS.UnloadDown = GSR.AxisUnitPosition[AxHit];
			break;
			
//		case 20://���߻���λ
//			GPOS.PullBack_2 = GSR.AxisUnitPosition[AxPull_2];
//			break;
//		
//		case 21://����ȡ��λ
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
		case 1://����Ԥ��λ
			AxAbsUnit(AxFlow,10,GPOS.FlowReady);
			break;
		
		case 2://���̶���
			HZ_AxSetCurPos(AxFlow,GPOS.FlowReady - GPOS.FlowFeed);
			AxRelUnit(AxFlow,10,GPOS.FlowFeed);
			break;
		
		case 3://����ȡ��λ
			AxAbsUnit(AxPickZ,10,GPOS.PickZTake);
			break;
		
		case 4://���ܷ���λ
			AxAbsUnit(AxPickZ,10,GPOS.PickZPut);
			break;
		
		case 5://���߻���λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPull,10,GPOS.PullBack);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 6://����ȡ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPull,10,GPOS.PullTake);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 7://ǰ��ȡ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLF,10,GPOS.PickTakeLF);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 8://ǰ�����λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLF,10,GPOS.PickPutLF);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 9://ǰ��ȡ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRF,10,GPOS.PickTakeRF);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 10://ǰ�ҷ���λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRF,10,GPOS.PickPutRF);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 11://����ȡ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLB,10,GPOS.PickTakeLB);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 12://�������λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickLB,10,GPOS.PickPutLB);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 13://����ȡ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRB,10,GPOS.PickTakeRB);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 14://���ҷ���λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				AxAbsUnit(AxPickRB,10,GPOS.PickPutRB);
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 15://����Ԥ��λ
			AxAbsUnit(AxPickZ,10,GPOS.PickZReady);
			break;
		
		case 16://������λ
//			AxAbsUnit(AxWeld,10,GPOS.WeldReady);
			break;
		
		case 17://������λ
//			AxAbsUnit(AxWeld,10,GPOS.WeldWork);
			break;
				
		case 18://������
//			AxAbsUnit(AxHit,10,GPOS.UnloadUp);
			break;
		
		case 19://������
//			AxAbsUnit(AxHit,10,GPOS.UnloadDown);
			break;
			
//		case 20://���߻���λ
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				AxAbsUnit(AxPull_2,10,GPOS.PullBack_2);
//			else
//				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
//			break;
//		
//		case 21://����ȡ��λ
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				AxAbsUnit(AxPull_2,10,GPOS.PullTake_2);
//			else
//				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
//			break;
		
		case 100://λ�ƻ���+Ԥ��λ
			LT.AxGohomeReady[AxFlow].execute = 1;
			break;
		
		case 101://���ܻ���+Ԥ��λ
			LT.AxGohomeReady[AxPickZ].execute = 1;
			break;
		
		case 102://���߻���+Ԥ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPull].execute = 1;
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 103://��ǰ����+Ԥ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickLF].execute = 1;
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 104://��ǰ����+Ԥ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickRF].execute = 1;
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 105://������+Ԥ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickLB].execute = 1;
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 106://�Һ����+Ԥ��λ
			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
				LT.AxGohomeReady[AxPickRB].execute = 1;
			else
				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
			break;
		
		case 107://���߻���+Ԥ��λ
//			LT.AxGohomeReady[AxWeld].execute = 1;
			break;
		
		case 108://���������+Ԥ��λ
//			LT.AxGohomeReady[AxHit].execute = 1;
			break;
			
//		case 109://����2����+Ԥ��λ
//			if(fabs(GPOS.PickZReady - GSR.AxisUnitPosition[AxPickZ]) < 50)
//				LT.AxGohomeReady[AxPull_2].execute = 1;
//			else
//				AlarmSetBit(2,0,7);//�뽫���ܻص�Ԥ��λ�ٽ�����������
//			break;
		
	}
	if(GUW.GotoPos)
	{
		GUR.HaveToReset = 1;
	}
	GUW.GotoPos = 0;
}

