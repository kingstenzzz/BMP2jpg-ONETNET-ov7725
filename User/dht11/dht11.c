#include "dht11.h"
#include "./systick/bsp_SysTick.h"


void DHT11_delay_us(u32 time)
{ 
	u16 i=0;  
   while(time--)
   {
      i=10;  //�Լ�����
      while(i--) ;    
   }

}

void DHT11_delay_ms(u32 z)
{
  u32 i=1000*z;
	DHT11_delay_us(i);

}




void DHT11_Init(void)
{
	DHT11_GPIO_Config();	//DHT11��GPIO�˿ڳ�ʼ��
	
	DHT11_Dout_1;			//����PE.6
}


/*
************************************************************
*	�������ƣ�	DHT11_GPIO_Config
*	�������ܣ�	����DHT11�õ���I/O��
*	��ڲ�����	��
*	���ز�����	��
*	˵����		PE.6�������
************************************************************
*/
void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	�������ƣ�	DHT11_Mode_IPU
*	�������ܣ�	ʹDHT11-DATA���ű�Ϊ��������ģʽ
*	��ڲ�����	��
*	���ز�����	��
*	˵����		PE.6��������
************************************************************
*/
void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	�������ƣ�	DHT11_Mode_Out_PP
*	�������ܣ�	ʹDHT11-DATA���ű�Ϊ�������ģʽ
*	��ڲ�����	��
*	���ز�����	��
*	˵����		PE.6�������
************************************************************
*/
void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	�������ƣ�	DHT11_ReadByte
*	�������ܣ�	��DHT11��ȡһ���ֽڣ������Ч�ֽڣ�MSB������
*	��ڲ�����	��
*	���ز�����	temp��������ֵ
*	˵����		  
************************************************************
*/
uint8_t DHT11_ReadByte(void)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	
	for(i = 0; i < 8; i++)
	{
		//ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ�������50us�͵�ƽ����
		while(DHT11_Dout_IN == Bit_RESET){};
			
		/*DHT11 ��50us�ĵ͵�ƽ + 26~28us�ĸߵ�ƽ��ʾ��0������50us�ĵ͵�ƽ + 70us�ߵ�ƽ��ʾ��1����
		ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ*/
		DHT11_delay_us(35);								//��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��
		
			
		if(DHT11_Dout_IN == Bit_SET)				//x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� 
		{
			while(DHT11_Dout_IN == Bit_SET){};		//�ȴ�����1�ĸߵ�ƽ����
					
			temp |= (u8)(0x01 << (7 - i));			//�ѵ�7-iλ��1��MSB����
		}
		else										// x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{
			temp &= (u8)~(0x01 << (7 - i));			//�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}


/*
************************************************************
*	�������ƣ�	DHT11_Read_TempAndHumidity
*	�������ܣ�	��DHT11��ȡһ���ֽڣ������Ч�ֽڣ�MSB������
*	��ڲ�����	DHT11_Data����ʪ�Ƚṹ��
*	���ز�����	SUCCESS(��ERROR)
*	˵����		һ�����������ݴ���Ϊ40bit����λ�ȳ�
*			8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У���
*			DHT11Ϊ�ӻ���΢������Ϊ������ֻ���������дӻ����ӻ�����Ӧ�� 
************************************************************
*/
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
	DHT11_Mode_Out_PP();				//������Ϊ���
	DHT11_Dout_0;						//��������
	DHT11_delay_ms(18);						//��ʱ18ms

	DHT11_Dout_1;						//��������
	DHT11_delay_us(30);						//������ʱ30us

	DHT11_Mode_IPU();					//������Ϊ���룬�жϴӻ���Ӧ�ź�

	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������   
	if(DHT11_Dout_IN == Bit_RESET)     
	{
		//��ѯֱ���ӻ�����80us�͵�ƽ ��ΪӦ���ź� 
		while(DHT11_Dout_IN == Bit_RESET){};

		//��ѯֱ���ӻ�������80us�ߵ�ƽ ֪ͨ΢������׼����������
		while(DHT11_Dout_IN == Bit_SET){};

		//��ʼ��������   
		DHT11_Data->humi_int = DHT11_ReadByte();
		DHT11_Data->humi_deci = DHT11_ReadByte();
		DHT11_Data->temp_int = DHT11_ReadByte();
		DHT11_Data->temp_deci = DHT11_ReadByte();
		DHT11_Data->check_sum = DHT11_ReadByte();

		DHT11_Mode_Out_PP();			//��ȡ���������Ÿ�Ϊ���ģʽ
		DHT11_Dout_1;					//��������

		//����ȡ�������Ƿ���ȷ
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
		{
			return SUCCESS;
		}
		else 
		{    
			return ERROR;
		}
	}
	else
	{
		return ERROR;
	}
}

