

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "./usart/bsp_usart.h"
#include "hwtimer.h"
#include "onenet.h"

//C��
#include <string.h>
#include <stdio.h>

#ifdef ESP8266

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"192.168.1.102\",876\r\n"



unsigned char esp8266_buf[1024];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


void  SendCmd(char* cmd, char* result,int timeout)
{
	while(1)
	{
		
	 Usart_SendString(USART3, (uint8_t*)cmd, strlen(cmd));
		mDelay(timeout);
	//	UsartPrintf(USART1,"%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, esp8266_buf);
		 if((NULL != strstr((const char *)esp8266_buf, (const char *)result)))	//�ж��Ƿ���Ԥ�ڵĽ��
        {
            break;
        }
        else
        {
            mDelay(100);
						UsartPrintf(USART1,( char*)esp8266_buf);
        }
	}
}


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}


//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	
	
	unsigned char timeOut = 200;

	Usart_SendString(USART3, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
	
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
				//UsartPrintf(USART1,"%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, esp8266_buf);
			
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
		//	ESP8266_Clear();									//��ջ���
				
				return 0;
			}
			printf((const char *)esp8266_buf);
	}
		
		mDelay(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART3, data, len);		//�����豸������������
	}
//	printf((const char *)esp8266_buf);
	

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	while(timeOut--)
	
	{
		ESP8266_WaitRecive();
		while(ESP8266_WaitRecive()==REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
					
				}
				else
					return NULL;
				
			}
		}
		
		mDelay(5);													//��ʱ�ȴ�
	} 
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	//ESP8266��λ����
	/*
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOD, GPIO_Pin_0, Bit_RESET);
	mDelay(250);
	GPIO_WriteBit(GPIOD, GPIO_Pin_0, Bit_SET);
	mDelay(600);
	*/
	ESP8266_Clear();

	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd(AT, "OK"))
		mDelay(500);
	
	
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd(CWMODE, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd(RST, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "3. CWJAP\r\n");
	while(ESP8266_SendCmd(CWJAP, "OK"))
		mDelay(500);
		UsartPrintf(USART_DEBUG, "3. CIPMODE\r\n");
	while(ESP8266_SendCmd(CIPMODE0, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "4. CIPSTART\r\n");
	while(ESP8266_SendCmd(CIPSTART, "CONNECT"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "5. ESP8266 Init OK\r\n");

}

//==========================================================
//	�������ƣ�	USART3_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	
			esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART3->DR;
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		
	}

}


void ESP8266_EnterTrans(void)
{
	printf("����͸��\r\n");

	ESP8266_SendCmd(CIPMODE1, "OK");	//����͸��ģʽ
	ESP8266_SendCmd(CIPSEND, "OK" );	//����͸��ģʽ
	ESP8266_SendCmd("AT+CIPSEND\r\n", ">");	//��������
						//�ȴ�100ms

}

void ESP8266_QuitTrans(void)
{
    SendCmd(QUITTRANS,"+++",100);			//�ȴ�100ms
printf("�˳�͸��\r\n");

}


u8 Check_Wifi(void )
{
		ESP8266_SendCmd(CIPSTATUS,"STATUS");
	if(strstr((const char *)esp8266_buf,"STATUS:3"))
	{
		printf("wifi is  normal\n");
		return 0;				
	}
	else if(strstr((const char *)esp8266_buf,"STATUS:4")) //ʧȥ����  2
	{
		printf("tcp link lost\n");
		return 2;
		
	}	
	else if(strstr((const char *)esp8266_buf,"STATUS:5"))
	{
		printf("wifi  is unusable\n");
		return 1;		
	}
	else
	{
		return 3;
	}
	
}


void reLink(void )
{
	UsartPrintf(USART_DEBUG, "4. CIPSTART\r\n");
	while(ESP8266_SendCmd(CIPSTART, "CONNECT"))
		mDelay(500);
   OneNet_DevLink();
}

#endif


