#include "timer.h"
#include "stm32f10x.h"
#include "delay.h"

void GPIO(void)
{
	//�����ø��ù��ܣ�������ű任
	
	  GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_8); 
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
		GPIO_Init(GPIOB,&GPIO_InitStructure);		

}

//arr�����þ��Ǽ������ڣ��൱��CNT��ֵ
//psc���Ǽ���һ�ε�ʱ��
//�����һ������Ϊ20ms�ķ�����arrΪ199��pscΪ7199������һ��0.1ms��
void TIM4_PWM_Init()
	{
	  
		GPIO_InitTypeDef GPIO_InitStructure;   //����һ���ṹ�������������ʼ��GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//����һ���ṹ�������������ʼ����ʱ��

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;// PC6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//TIM4��ʱ����ʼ��
	TIM_TimeBaseInitStructure.TIM_Period = 199; //PWM Ƶ��=72000/(199+1)=36Khz//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//�ı�ָ���ܽŵ�ӳ��	//pC6

	//PWM��ʼ��	  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	//ע��˴���ʼ��ʱTIM_OC1Init������TIM_OCInit������������Ϊ�̼���İ汾��һ����
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܻ���ʧ��TIMx����
	 TIM_SetCompare2(TIM3, 195);
	
	mDelay(500);
	TIM_SetCompare2(TIM3, 175);
			 TIM_SetCompare2(TIM3, 195);
	
	mDelay(500);
	TIM_SetCompare2(TIM3, 175);

	
	}
	
	u16 Get_angle(int jiaodu)//����ֵ����һ��Ҫ��u16     ��Ϊarr��ֵ����һ���ֽ�
	{
		 u16 zkb=0;
		//0.5/45=time/0.18   ==>   time=0.002ms  ==>ÿת��0.18�Ƕ���ʱ0.002ms=2us
//    zkb=jiaodu*14+250;//�����
		if(jiaodu>0)
		zkb=185+jiaodu*0.11;//250��2us+���Ƕ�/���ȣ�=ռ�ձ�
		else
			{
		jiaodu=-jiaodu;
		zkb=185-jiaodu*0.11;
			}
		return zkb;  
	} 	
	
	
	void Set_angle(_Bool dir,u8 angle)
 {
	static u8 pre_angel=0;
	 if(dir)
	 {
		 pre_angel=pre_angel+angle;
		 TIM_SetCompare3(TIM3, Get_angle(pre_angel));
		 
	 }
	  else
	 {
		 		 pre_angel=pre_angel-angle;

		 TIM_SetCompare3(TIM3, Get_angle(pre_angel));
		 
	 }
	 
 }
	
	

	
	
	
	

	