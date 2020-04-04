#include "warning.h"
#include "hwtimer.h"
#include "beep.h"
#include "iic.h"



enum 
{
	Safe=0,
	Fire,
	Hot,
	Cold,
	Wet,
	Dry
}alarm_mode;

extern int16_t temp_max;
extern int16_t temp_min;
extern u8 humi_max;
extern u8 humi_min;
extern _Bool fire;


//��ȡ���ݶ�LED�ƺͷ��������п���
void WARNING(uint8_t flag)
{
	switch (flag)
	{
		case Safe:
			LED_RGBOFF		//��ȫģʽ	
		break;				
		
		case Fire:                     
			Beep_Set(BEEP_ON);
		break;						
		case Hot:	
			LED_RED		//����ģʽ�������	
		break;
		case Cold:	
			LED_BLUE	//����ģʽ��������	
		break;		
		case Wet:
			LED_CYAN;
		break;
		case Dry:
			LED_PURPLE;
		break;
		default: break;
	}
}


/*
************************************************************
*	�������ƣ�	DHT11_Check
*	�������ܣ�	��ʪ��Խ�޼��
*	��ڲ�����	��
*	���ز�����	��
*	˵����		
************************************************************
*/
void DHT11_Check(DHT11_Data_TypeDef *DHT11_Data, int16_t temp_max, int16_t temp_min, uint16_t humi_max, uint16_t humi_min)
{
	//�¶�Խ�޼��
	if(DHT11_Data->temp_int >= temp_max)		//��ǰ�¶ȴ��ڵ�������¶�ʱ,���¾���
	{
		alarm_mode=Hot;
		WARNING(Hot);
		
	}			
	else if(DHT11_Data->temp_int <= temp_min)	//��ǰ�¶�С�ڵ�������¶�ʱ,���¾���
	{          
		alarm_mode=Cold;
		WARNING(Cold);

	}
	else										//��ȫ
	{             
		alarm_mode=Safe;
		WARNING(Safe);

    }
	
	//ʪ��Խ�޼��
	if(DHT11_Data->humi_int >= humi_max)		//��ǰ�¶ȴ��ڵ�������¶�ʱ,���¾���
	{
             
   alarm_mode=Wet;
		WARNING(Wet);
	}			
	else if(DHT11_Data->humi_int <= humi_min)	//��ǰ�¶�С�ڵ�������¶�ʱ,���¾���
	{
		alarm_mode=Dry;
		WARNING(Dry);
	}

}






/*
************************************************************
*	�������ƣ�	Fire_Check
*	�������ܣ�	�����ⱨ������
*	��ڲ�����	fire�������־
*	���ز�����	��
*	˵����		 
************************************************************
*/
void Fire_Check(u8 *fire)
{
	if(FIRE_Data_IN() == 1)								//�������ź�Ϊ0ʱ���л���
	{		
		UsartPrintf(USART_DEBUG, "���澯��\r\n");
		*fire = 1;			
		alarm_mode=Fire;
		WARNING(Fire);
		mDelay(500);
	}
	else if(FIRE_Data_IN() == 0)						//�������ź�Ϊ1ʱ���޻���
	{
		*fire = 0;
		Beep_Set(BEEP_OFF);
	}
}



void Check_sensor(volatile data_Stream *data_stram)
{
	Fire_Check(&fire);
	DHT11_Data_TypeDef DH11_data;
	if(DHT11_Read_TempAndHumidity(&DH11_data))
	{
		DHT11_Check(&DH11_data,temp_max,temp_min,humi_max,humi_min);
			UsartPrintf(USART_DEBUG, "--------------�������--------------\r\n");			
			UsartPrintf(USART_DEBUG, "�¶ȣ�%d ��\r\n", DH11_data.temp_int);
			UsartPrintf(USART_DEBUG, "ʪ�ȣ�%d ��RH\r\n", DH11_data.humi_int);
			UsartPrintf(USART_DEBUG, "����¶ȣ�%d\r\n", temp_max);
			UsartPrintf(USART_DEBUG, "����¶ȣ�%d\r\n", temp_min);
			UsartPrintf(USART_DEBUG, "���ʪ�ȣ�%d\r\n", humi_max);
			UsartPrintf(USART_DEBUG, "��Сʪ�ȣ�%d\r\n", humi_min);	
		  data_stram->humidit=DH11_data.humi_int;
	    data_stram->temp=DH11_data.temp_int;
	}
			printf("����%d",fire);
			data_stram->fire=fire;
	    
			if(data_stram->tem_max!=temp_max)
			{
					data_stram->tem_max=temp_max;
					ee_WriteBytes((u8)temp_max , 0x02);

			}
			else if(data_stram->tem_min!=temp_min)
				{
						data_stram->tem_min=temp_min;
				ee_WriteBytes((u8)temp_min, 0x03);
					
				}
			else if(data_stram->hum_max!=humi_max)
				{
							data_stram->hum_max=humi_max;
							ee_WriteBytes((u8)humi_max , 0x00);
				}
				else if (data_stram->hum_min!=humi_min)
			{
	
			data_stram->hum_min=humi_min;
			ee_WriteBytes((u8)humi_min, 0x01);
		
		
			}
	
	
	
}


