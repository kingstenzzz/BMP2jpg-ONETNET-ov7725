
#include "iwdg.h"

void IWDG_Config(uint16_t prv,uint16_t rlv)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//����
	IWDG_SetPrescaler(prv);//��Ƶ0-255
	IWDG_SetReload(rlv);//��װֵ
	//4��Ƶ����װֵ624�����Ź����ʱ����Ϊ1000ms
	
	IWDG_ReloadCounter();//ι��
	IWDG_Enable();//ʹ�ܿ��Ź�
}


void Feed()
	{
		IWDG_ReloadCounter();
	}
	