
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

u16 link_status;
u8 link_no;
#define  RECV_DATA_MAX_SIZE  1024

   u8  M8266buf[RECV_DATA_MAX_SIZE];
   u16 received = 0;
	 u32 total_received = 0;
   u32 MBytes = 0;



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
	if(M8266WIFI_SPI_Has_DataReceived())
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
	      received = M8266WIFI_SPI_RecvData(M8266buf, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &link_status);

			  if(  (link_status&0xFF)!= 0 )  
			  {
				    if( (link_status&0xFF)==0x22 )      // 0x22 = Module buffer has no data received
				    {  
			         return 1;
				    }
				    else if( (link_status&0xFF)==0x23 )   
				    { 
							return 0;
							// (Chinese: �ϴε��ý��պ���M8266WIFI_SPI_RecvData()ʱ����δ�������ϴ��Ǹ�����������ε��ü�������֮ǰ�İ���������������һЩ���������罫һ�ν��ջ����������󳤶����޼Ӵ�
				    }
				    else if(  (link_status&0xFF)==0x24)   
				    { 
							return 3;
	            // the packet is large in size than max_len specified and received only the max_len // TCP?????????
							// normally caused by the burst transmision by the routers after some block. 
							// Suggest to stop the TCP transmission for some time
					    // do some work here if necessary
							// (Chinese: ģ�������յ������ڱ���ȡ��������ĳ��ȣ������������max_len������ָ���ĳ��ȡ�ͨ������ΪԶ���󷢷��ͻ�·����������ʱ�����˴����ճ�����µ���ģ��İ�������
							//           ����Զ��ʵ�ʷ��͵ľ���һ���������䳤�ȳ�����������ָ������󳤶����ޡ������ǰ�ߵ�ԭ�򣬽�����ͣԶ��TCPͨ��һ��ʱ�䡣����Ǻ��ߣ�����Ӵ�max_len����ֵ����
							//           �����κδ�����������ʱ����Ƭ������߽��յ��ĳ����ᱻ��ɶ��С����Ҫ�����ٴ��ƽ⡣
							//           ��Ҫʱ��������Ӧ�Ĵ���
				    }
					
	
   }
					else
						{
							return 0;
					    printf("����ʧ��");
						}
				
				}
			return 0;
			}



unsigned char *M8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;


		
		if(M8266_WaitRecive()==REV_OK)								//����������
		{
						return (unsigned char *)(M8266buf);
		
		}


}

