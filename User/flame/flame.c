#include "flame.h"


/*
************************************************************
*	�������ƣ�	Fire_Init
*	�������ܣ�	���û��洫�����õ���I/O��
*	��ڲ�����	��
*	���ز�����	��
*	˵����	PD5
************************************************************
*/
void Fire_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//�������� 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
