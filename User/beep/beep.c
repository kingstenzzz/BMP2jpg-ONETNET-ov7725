#include "beep.h"

BEEP_INFO beepInfo = {0};


/*
************************************************************
*	�������ƣ�	Beep_Init
*	�������ܣ�	��������ʼ��
*	��ڲ�����	��
*	���ز�����	��
*	˵����		BEPP-PA.8
************************************************************
*/
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				

	//��ʼ����ɺ󣬹رշ�����
	Beep_Set(BEEP_OFF);
}


/*
************************************************************
*	�������ƣ�	Beep_Set
*	�������ܣ�	����������
*	��ڲ�����	status�����ط�����
*	���ز�����	��
*	˵����		��-BEEP_ON	��-BEEP_OFF
************************************************************
*/
void Beep_Set(_Bool status)
{
	//���status����BEEP_ON���򷵻�Bit_SET�����򷵻�Bit_RESET
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, status == BEEP_ON ? Bit_SET : Bit_RESET);
	
	beepInfo.beep_status = status;
}
