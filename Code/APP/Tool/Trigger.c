#include "Trigger.h"
//´¥·¢Æ÷
s32	TrigOne(TrigDef *this, s32 Condition, u32 Time)
{
	if(Condition == 0)
	{
		this->TrigFlag = 1;
		this->TrigTimeStamp = SysTim_GetUser();
	}
	if(Condition != 0 && this->TrigFlag == 1 && (SysTim_GetUser() - this->TrigTimeStamp)/10 > Time)
	{
		this->TrigFlag = 0;
		return 1;
	}
	return 0;
}
//´¥·¢Æ÷¸´Î»
void TrigRst(TrigDef *this)
{
	this->TrigFlag = 0;
	this->TrigTimeStamp = SysTim_GetUser();
}



