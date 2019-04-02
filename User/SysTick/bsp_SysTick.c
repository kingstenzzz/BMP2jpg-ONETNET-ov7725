#include "delay.h"

static __IO u32 TimingDelay;
__IO uint32_t g_ul_ms_ticks=0;



/*
************************************************************
*	�������ƣ�	SysTick_Init
*	�������ܣ�	����ϵͳ�δ�ʱ�� SysTick
*	��ڲ�����	��
*	���ز�����	��
*	˵����		  
************************************************************
*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	* SystemFrequency / 100000	 10us�ж�һ��
	* SystemFrequency / 1000000  1us�ж�һ��
	*/
	//if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if(SysTick_Config(SystemCoreClock / 1000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while(1);
	}
	// �رյδ�ʱ��  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}


/*
************************************************************
*	�������ƣ�	Delay_us
*	�������ܣ�	us��ʱ����,10usΪһ����λ
*	��ڲ�����	nTime: Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
*	���ز�����	��
*	˵����		  
************************************************************
*/
void Delay_us(__IO uint32_t nTime)
{ 
	TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}


/*
************************************************************
*	�������ƣ�	TimingDelay_Decrement
*	�������ܣ�	��ȡ���ĳ���
*	��ڲ�����	��
*	���ز�����	��
*	˵����		�� SysTick �жϺ��� SysTick_Handler()����		  
************************************************************
*/
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

