 /*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 8��ײ�
 * Description:�û��߼�
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"
#include "math.h"

#define PI 3.1415926


/*ȫ�ֱ�����ͳһ����*/
Task LogicTask = { 0 }; //ͳһ��������
SysData Data = { 0 };  //ͳһ��������
Gpio I_BeltSupply[2] = {I_BeltSupplyL,I_BeltSupplyR};
Gpio Q_BeltSupply[2] = {Q_BeltSupplyL,Q_BeltSupplyR};
//u32	AxBelt[2] = {AxBelt_1,AxBelt_2};



#define	AxPickPPR	2000


//�豸������
UphDef DeveceUph;
void DeviceMain(LogicParaDef *p)
{
	static u32 Tm_I_EntranceInp,I_EntranceInp_Ok;
	//�ⲿ�����ṩ�豸�������źſ���
	if(FSM_STA == RUN)//��̨����״̬
	{			
		if(GCFG.EntranceInpEn == 0)
		{
			Q_Set(Q_MaskSupply,OFF);
		}
		else
		{
			Q_Set(Q_MaskSupply,ON);//�����ⲿ���ֻ�����
		}
		Q_Set(Q_ExitMotor,ON);
		my.execute = 1;
		LT.SectionInpTask.execute = 1;
	}
	else
	{
		PARAINIT(LT.SectionInpTask);
	}
	//һֱ������״̬��������˲�����Ҫ�ȴ����̿�ʼ�ܵ�ʱ���ټ�⣬����ʱ��
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
		case 1://�ȴ�����п��֣�λ�����ƶ�һ��
			if((I_EntranceInp_Ok == 1 || GCFG.EntranceInpEn == 0))
			{
				AxRelUnit(AxFlow,GSPD,GPOS.FlowFeed);
				my.step = 2;
			}
			break;
			
		case 2://λ�Ƶ�λ��ִ�����Ϻͺ��Ӷ���
			if(AxSta(AxFlow) == AXSTA_READY)
			{
				HZ_AxSetCurPos(AxFlow,GPOS.FlowReady);  
				LT.WeldTask.execute = 1;
				my.step = 3;
			}
			break;
			
		case 3://������ɺ󣬼�����ܣ�����
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

//ȡ�ߣ�ȡ���õ��ߣ�����λ�Ϸ��ȴ�
void Pick(LogicParaDef *p)
{
	static u32 Tm,Tm2;
	TASK_PROCESS
	{
		case 1:
			Q_Set(Q_PickClamp,OFFR);//�ſ�����
			TimerRst(&Tm);//��ʱ
			my.step = 2;
			break;
			
		case 2://�����ᵽԤ��λ
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
		
		case 3://�����ᵽλ��ǰ�����ҵ�ȡ��λ
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
			
		case 4://ǰ�����ҵ�λ
			if(AxInpSta(AxPickLB) == AXSTA_READY &&
			   AxInpSta(AxPickLF) == AXSTA_READY &&
			   AxInpSta(AxPickRB) == AXSTA_READY &&
			   AxInpSta(AxPickRF) == AXSTA_READY)
			{
				//�������û��ɣ���һ����
				if(LT.PullTask.done != 1)
				{
					LT.PullTask.execute = 1;
				}
				my.step = 5;
			}
			break;
			
		case 5:
			if(LT.PullTask.execute == 0)//���߽���
			{
				if(LT.PullTask.done == 1)//�ж����߽����OK����
				{
					LT.PullTask.done = 0;
					my.step = 6;
				}
				else//NG����������ʧ�ܣ��������
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 6://�����ᵽȡ��λ
			if(AxSta(AxPickZ) == AXSTA_READY)
			{
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZTake);
				my.step = 7;
			}
			break;
			
		case 7://�����ᵽλ
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY)
			{
				TimerRst(&Tm);
				Q_Set(Q_PickClamp,ONR);//ȡ�߼мн�
				my.step = 8;
			}
			break;
			
		case 8://��ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.PickClampOnDelay)
			{
				Q_Set(Q_PullClamp,OFFR);//���߼����ſ�
				Q_Set(Q_Cut,ON);//��������
				TimerRst(&Tm);
				TimerRst(&Tm2);
				my.step = 9;
			}
			break;
			
		case 9://��ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.CutOnDelay)
			{
				Q_Set(Q_Cut,OFF);//�����ſ�
				TimerRst(&Tm);
				my.step = 10;
			}
			break;
			
		case 10://��ʱ��
			if(TimerCnt(&Tm2,1) >= GDLY.PullClampOffDelay && TimerCnt(&Tm,1) >= GDLY.CutOffDelay)
			{
//				AxRelUnit(AxPickZ,GSPD,AxPickPPR - GPOS.PickZTake + GPOS.PickZReady);//�����ᵽ��λ��
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZReady);
				my.step = 11;
			}
			break;
			
		case 11://��������λ�ú�
			if(AxSta(AxPickZ) == AXSTA_READY)
			{
				LT.PullTask.execute = 1;//����
				my.step = 12;
			}
			break;
			
		case 12://�ᶼ��Ԥ��״̬���ҵ㺸��������λ��
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY) 
				//&& AxSta(AxWeld) == AXSTA_READY)
			{
				//ǰ�����ҵ�����λ
//				HZ_AxSetCurPos(AxPickZ,GPOS.PickZReady);
				AxAbsUnit(AxPickLB,GSPD,GPOS.PickPutLB);
				AxAbsUnit(AxPickLF,GSPD,GPOS.PickPutLF);
				AxAbsUnit(AxPickRB,GSPD,GPOS.PickPutRB);
				AxAbsUnit(AxPickRF,GSPD,GPOS.PickPutRF);
				my.step = 13;
			}
			break;
			
		case 13://ǰ�����ҵ�λ�󣬽���
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

//����
void Weld(LogicParaDef *p)
{
	static u32 Tm;
	static u32 Tm_WeldUp,Tm_WeldDown;
	//�㺸���ױ���
	if(TimerCnt(&Tm_WeldDown,Q_Get(Q_WeldLift) == ON && I_Get(I_WeldCydDown) != ON) > 800)
	{
		AlarmSetBit(2,0,4);//�㺸�����¸�Ӧ�쳣
		PARAINIT(LT.WeldTask);
	}
	if(TimerCnt(&Tm_WeldUp,Q_Get(Q_WeldLift) == OFF && I_Get(I_WeldCydUp) != ON) > 800)
	{		
		AlarmSetBit(2,0,5);//�㺸�����ϸ�Ӧ�쳣
		PARAINIT(LT.WeldTask);
	}
	
	TASK_PROCESS
	{
		case 1://��ʱ
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://��ǰ�������
			if(my.count || GCFG.EntranceInpEn == 0)
			{
				if(my.count > 0)//my.count������Ϊ��ǰ��⵽�ϵı�־
				{
					my.count--;
				}
				my.step = 3;//ȥ����
			}
			else
			{
				my.step = 10;//ȥ����
			}
			break;
		
		case 3://��ѯ�Ƿ�ȡ������
			if(LT.PickTask.done != 1)
			{
				LT.PickTask.execute = 1;//û�о�ȥȡ
			}
			my.step = 4;
			break;
			
		case 4://ȷ��ȡ�����
			if(LT.PickTask.execute == 0 ) //&& AxSta(AxWeld) == AXSTA_READY)
			{
				if(LT.PickTask.done == 1)
				{
					LT.PickTask.done = 0;
					AxAbsUnit(AxPickZ,100,GPOS.PickZPut);//�������½�
					Q_Set(Q_WeldLift,ON);
					//AxAbsUnit(AxWeld,GSPD,GPOS.WeldWork);//�㺸�����½�
					if(GCFG.WeldEn == 1)
					{
						Q_Set(Q_WeldLeft,ON);//��������
						Q_Set(Q_WeldRight,ON);//��������
					}
					TimerRst(&Tm);
					my.step = 5;
				}
				else//ȡ��ʧ�ܣ��������������
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 5://�ȴ�������͵㺸�����µ�λ
			if(AxSta(AxPickZ) == AXSTA_READY && (I_Get(I_WeldCydDown)==ON || AlarmGetBit(2,0,4)))//AxSta(AxWeld) == AXSTA_READY)
			{
				LT.PickTask.execute = 1;//ȡ���ֿ���ȥȡ����
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://�ȴ���������ʱ����
			if(TimerCnt(&Tm,1) >= GDLY.WeldOnDelay)
			{
				Q_Set(Q_WeldLeft,OFF);//�رճ�����
				Q_Set(Q_WeldRight,OFF);//�رճ�����
				TimerRst(&Tm);
				my.step = 7;
			}
			break;
			
		case 7://�ȴ��̻�ʱ��
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
		
		case 9://�ȴ�����������ߵ�ǰλ����Ԥ��λС���г̵�1/3ʱ
			if( (I_Get(I_WeldCydUp) == ON  || (TimerCnt(&Tm,1) > GDLY.WeldLiftOffDelay)) && FSM_STA == RUN)
			{
				my.step = 10;//ȥ����
			}
			break;
			
		case 10:
			if(I_Get(I_WeldInp) == ON)//���һ�º�ǰ���ϼ��
			{
				my.count++;//���Ͼ�++
			}
			my.execute = 0;
			my.step = 0;
			break;
	}
}

//����
void Pull(LogicParaDef *p)
{
	static u32 Tm,Tm2;
	static u32 I_BeltSupply_Off[2];
	//������߸�Ӧ�����߹�������û��OFF�źţ��оʹ�����������
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
		case 1://���߼����ſ��������ᵽȡ��λ
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
		
		case 2://���Ӽн�
			if(AxSta(AxPull) == AXSTA_READY  && TimerCnt(&Tm2,1) >= GDLY.DelayPull)
			{
				Q_Set(Q_PullClamp,ONR);
				TimerRst(&Tm);
				TimerRst(&Tm2);
				my.step = 3;
			}
			break;
			
		case 3://ѹ���ɿ�
			if(TimerCnt(&Tm,1) >= GDLY.PullClampOnDelay)
			{
				Q_Set(Q_Press,ON);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://�����ᵽ���ߺ�λ
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
			
		case 5://����һ��ʱ��֮�󣬻�������������ˣ���ѹ��
			if(AxSta(AxPull) == AXSTA_READY )
			{
				//Q_Set(Q_Press,OFF);
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://���ߵ���ߵ����ߺ�λ�ú���ʱѹ��
			if(TimerCnt(&Tm,1) >= GDLY.DelayPress)
			{
				Q_Set(Q_Press,OFF);
				TimerRst(&Tm);
				my.step = 7;
			}
			break;
			
		case 7://ѹ�����׶���ʱ�� �� ��������������̽���
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
						AlarmSetBit(2,0,6);//�����߶���ʧ��
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

//����
void BeltSupply(LogicParaDef *p, s32 i)
{
	static u32 Tm[2];
	//���������ź�û�и�Ӧʱ
	if(I_Get(I_BeltSupply[i]) == OFF)
	{
		my.execute = 1;
		if(GCFG.TrembleEn == 1)
			  LT.TrembleTask.execute = 1;
	}
	TASK_PROCESS
	{		
		case 1://ÿ�����ߺ��ִ��һ�ι��ߣ�������ߺ����ź�û�и�Ӧ
			if(I_Get(I_BeltSupply[i]) == ON)
			{
				my.execute = 0;
				my.step = 0;
			}
			else
			{
				TimerRst(&Tm[i]);
				Q_Set(Q_BeltSupply[i],ON);//���ߵ����
				//AxRelUnit(AxBelt[i],100,999999);//���߲�����
				my.count = 0;
				my.step = 2;
			}
			break;
			
		case 2://�����¸�Ӧ���ź�
			if(I_Get(I_BeltSupply[i]) == ON)
			{
				Q_Set(Q_BeltSupply[i],OFF);//�رչ��ߵ��
				//AxStop(AxBelt[i]);//�رչ��߲���
//				HZ_AxSetCurPos(AxBelt[i],0);
				my.execute = 0;
				my.step = 0;
			}
//			else if(TimerCnt(&Tm[i],1) >= 5000 && GCFG.PullAlmEn == 1)//������ߵ�����г�������ʱ����
//			{
//				AxStop(AxBelt[i]);
//				Q_Set(Q_BeltSupply[i],OFF);//�رչ��ߵ��
//				AlarmSetBit(2,0,2+i);//���������ʹ����̫��
//				my.execute = 0;
//				my.step = 0;
//			}
			break;
	}
}

//���ϴ���
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
	if(TrigOne(&Tr,I_ExitCheck_val == ON && (FSM_STA == RUN || FSM_STA == D_RESET) && GUR.HitReady == 1, 20))//������ͨ�����ϸ�Ӧ
	{
		GUS.Product.Data.Cnt++;
		my.execute = 1;//ִ��һ�δ���
	}
	TASK_PROCESS
	{
		case 1:
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://�����£���ʱ
			if(TimerCnt(&Tm,1) >= GDLY.UnloadInp)
			{
				//AxRelUnit(AxHit,100,GPOS.UnloadDown);
				//AxAbsUnit(AxHit,100,GPOS.UnloadDown);
				Q_Set(Q_UnloadCyd,ON);
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
		
		case 3:////��ʱ����������
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOnDelay ) //&& AxSta(AxHit) == AXSTA_READY)
			{
				//AxAbsUnit(AxHit,100,GPOS.UnloadUp);
				Q_Set(Q_UnloadCyd,OFF);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://��ʱ���������������ִ��һ��Ƥ��
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOffDelay)// && AxSta(AxHit) == AXSTA_READY)
			{
				my.execute = 0;
				my.step = 0;
				my.count ++;
				if(my.count >= GCFG.UnloadPackNum)//�������ڴ������
				{
					my.count = 0;
					if(GCFG.UnloadPackNum > 0)
					{
						LT.UnloadBeltTask.execute = 1;//ִ��һ��Ƥ����
					}
				}
			}
			break;
	}
}

//����Ƥ��
void UnloadBelt(LogicParaDef *p)
{
	static u32 tm;
	TASK_PROCESS
	{
		case 1://��ʱ����
			TimerRst(&tm);
			my.step = 2;
			break;
		
		case 2://����Ƥ��
			if(TimerCnt(&tm,1) > GDLY.UnloadBeltDelay)
			{
				Q_Set(Q_CollectMotor,ON);
				TimerRst(&tm);
				my.step = 3;
			}
			break;
		
		case 3://��ʱ��ر�
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
		//һֱ������״̬��������˲�����Ҫ�ȴ����̿�ʼ�ܵ�ʱ���ټ�⣬����ʱ��
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
				AlarmSetBit(2,0,8);//��ڵ��ϱ���
				PARAINIT(LT.SectionInpTask);
			}			
			break;			
	}
}


Gpio I_Cloth[3] = {I_Cloth1,I_Cloth2,I_Cloth3};
//���Ͼ�ƫ
void RollCorrect()
{
	static u32 tm[3],tm1[3];
	for(int i = 0;i<3;i++)
	{
		if(TimerCnt(&tm1[i],I_Get(I_Cloth[i]) == OFF) > 50 && I_Get(I_MaskAuto) == ON && FSM_STA == RUN)
		{
			AlarmSetBit(2,0,7);//��Ƭ��ȱ����
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



//��Ƭ���߼�
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
		case 1://��ʱ����
			TimerRst(&tm);
			my.step = 2;
			break;
		
		case 2://����Ƥ��
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
		
		case 3://��ʱ��ر�
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
			Q_Set(Q_Cut,ON);//��������
			TimerRst(&Tm);
			my.step = 2;
			break;
			
		case 2://����ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.CutOnDelay)
			{
				Q_Set(Q_PullClamp,OFFR);//���߼����ſ�
				Q_Set(Q_Cut,OFF);//�����ſ�
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
			
		case 3://��������ʱ
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
		case 1://�ſ�����
			Q_Set(Q_PullClamp,OFFR);
			Q_Set(Q_PickClamp,OFFR);
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://���Ӽн�
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
//��������
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

//�����+Ԥ��λ
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

//��Ӧ�õ�������
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

//���豸�İ�ť����
void DeviceBtn()
{
	static TrigDef Tr_Start,Tr_Stop,Tr_Scram,Tr_Reset,Tr_Manual,Tr_Press;
	static u32 Tm_Scram;
	//��λǰ����ťִ�и�λ����λ�󣬰�ť����
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
	//��λǰ����ťִ�и�λ����λ�󣬰�ťֹͣ
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
	//��λ��ť
	if(TrigOne(&Tr_Reset,I_Get(I_Reset) == ON,50))
	{
		FSM_CMD = D_RESET;
	}
	//��ͣ����
	if(TrigOne(&Tr_Scram,I_Get(I_Scram) == ON,50))
	{
		FSM_CMD = STOP;
	}
	//��ͣ����
	if(TimerCnt(&Tm_Scram,I_Get(I_Scram) == OFF) > 50)
	{
		GUR.HaveToReset = 1;
		for(int i=0; i<PULS_NUM; i++)
		{
			AxStop(i);
		}
		FSM_CMD = SCRAM;
	}
	
	
	//��ť�е��ֶ���ֹͣ
	if(TrigOne(&Tr_Manual,I_Get(I_Manual) == OFF,50))
	{
		if(FSM_STA == RUN)
		{
			FSM_CMD = STOP;
		}
	}
	//ѹ����ť
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



//���豸�㶯����
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
//��ͣ��ִ��һ��
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
	for (i = 0; i < GPO_NUM; i++) //��ʼ�� �����
	{
        OutPut_SetSta(i, OFF);
	}
}

//������ִ��һ��
void start_handle()
{
	PARAINIT(DeveceUph);
}
//ֹͣ��ִ��һ��
void stop_handle()
{
	Q_Set(Q_MaskSupply,OFF);
	Q_Set(Q_ExitMotor,OFF);
}

//2020.7.20
//�豸�߼�ѭ��
void Logic()
{
	AxisDataConfig();				//����Ŀ�õ������÷�ʽ
	DeviceJogSet();					//����Ŀ�㶯����
	DeviceBtn();					//���豸�İ�ť����
	
    /******���������߼�*******/
	DeviceMain(&LT.DeviceMainTask);			//�豸������
	Pick(&LT.PickTask);						//ȡ�ߣ�ȡ���õ��ߣ�����λ�Ϸ��ȴ�
	Weld(&LT.WeldTask);						//����
	Pull(&LT.PullTask);						//����
	BeltSupply(&LT.BeltSupplyTask[0],0);	//������
	BeltSupply(&LT.BeltSupplyTask[1],1);	//������
	UnloadHit(&LT.UnloadHitTask);			//���ϴ���
	UnloadBelt(&LT.UnloadBeltTask);			//����Ƥ��
	SectionInp(&LT.SectionInpTask);			//��Ƭ���ϸ�Ӧ
	ResetToCut(&LT.ResetToCutTask);			//��λ����
	ManualDisBelt(&LT.ManualDisBeltTask);	//��λ����
	Tremble(&LT.TrembleTask);				//����
	/*******��Ƭ�߼�********/
	Slip(&LT.SlipTask);	
	
	for(int i=0; i<PULS_NUM; i++)
	{
		AxGohomeReady(&LT.AxGohomeReady[i],i);
	}
}

