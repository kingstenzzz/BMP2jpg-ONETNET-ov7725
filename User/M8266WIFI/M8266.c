
//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����


//Ӳ������
#include "delay.h"
#include "./usart/bsp_usart.h"
#include "hwtimer.h"
#include "onenet.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "brd_cfg.h"
#include "./led/bsp_led.h"




//C��
#include <string.h>
#include <stdio.h>

#define  RECV_DATA_MAX_SIZE  128

   u8  M8266buf[RECV_DATA_MAX_SIZE];
   u16 received = 0,pre_received=0;
	 u32 total_received = 0;
   u32 MBytes = 0;
	 
	u16 link_status;
	u8 link_no;
	u8  STA_Status;
	u8  TCP_Status;



void M8266HostIf_Init(void)
{
	 M8266HostIf_GPIO_CS_RESET_Init();
	
	 M8266HostIf_SPI_Init();
//	 M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);
	
}


void M8266_init()
{
	


	M8266HostIf_Init();
	while(!M8266WIFI_Module_Init_Via_SPI())
	{
		
		
		LED_RED;
		mDelay(500);
		
		LED_GREEN;
	
	}
	printf("wifi init\r\n");
	 while(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &link_status)==0)
  {
		LED_RED;
		mDelay(500);
		LED_GREEN;	
	}
	printf("wifi config\r\n");

	while(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, 80, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &link_status)==0)
	{		
		LED_RED;
		mDelay(500);
			printf("wifi connecting\r\n");
		LED_GREEN;	
	}

		
	
	
	
}

void M8266_SendData(unsigned char *data, unsigned short len)
{
	
	
	
}


u8 M8266_WaitRecive()
{
	
	if(received == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(received == pre_received)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		received = 0;							//��0���ռ���
	
		return REV_OK;								//���ؽ�����ɱ�־
	}		
	pre_received = received;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־
	

			
			}



unsigned char *M8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	if(M8266WIFI_SPI_Has_DataReceived())
	{
		received = M8266WIFI_SPI_RecvData(M8266buf, RECV_DATA_MAX_SIZE, 1000, &link_no, &link_status);
		return (unsigned char *)(M8266buf);
	
	}
	return NULL;


}

void M8266_Clear(void)
{

	memset(M8266buf, 0, sizeof(M8266buf));
	received = 0;

}



u8 M8266_NetCheck()
{
	u16 status;
	M8266WIFI_SPI_Get_STA_Connection_Status(&STA_Status,&status);
	if(status==0) printf("sta check faild");
	M8266WIFI_SPI_Query_Connection(link_no,NULL,&TCP_Status,NULL,NULL,NULL,&status);
	if(status==0) printf("tcp check faild");
	switch(STA_Status)
	{
		case 0:  printf("û������\r\n"); break;
		case 2: printf("����ʧ�ܣ��������\r\n");break;
		case 3: printf("����ʧ�ܣ�û��ָ��ssid\r\n");break;
		case 4: printf("����ʧ�ܣ�����ԭ��\r\n");break;
		case 5: printf("��������\r\n");break;
		
	}
	if(STA_Status!=5)
		return 1;
	else if(TCP_Status!=3)
	{
	printf("tcp���Ӷ�ʧ");
	return 2;
	}
	else
		return 0;

}

void M8266_ReLink(u8 status )
{
	if(status==1)//closed
	{
		while(!M8266WIFI_Module_Init_Via_SPI())
		{
			printf("��������wifi\r\n");
			mDelay(500);

	
		}
		
	}
	if(status==2) //lost
	{
		while(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, 80, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &link_status)==0)
		{		 
		  mDelay(500);
			printf("��������������\r\n");
		;
		}
		
	}
 
	
	UsartPrintf(USART_DEBUG, "��������\r\n");
	
}


