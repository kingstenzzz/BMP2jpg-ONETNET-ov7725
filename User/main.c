
  
#include "stm32f10x.h"
#include "./ov7725/bsp_ov7725.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./led/bsp_led.h"   
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./systick/bsp_SysTick.h"
#include "ff.h"
#include "encode.h"
#include "malloc.h"

//����Э���
#include "onenet.h"
#include "EdpKit.h"
//�����豸
#include "esp8266.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
//Ӳ������
#include "delay.h"
#include "hwtimer.h"
#include "warning.h"
#include "flame.h"
#include "dht11.h"
#include "beep.h"
#include "iic.h"
#include "iwdg.h"
#include "light_moto.h"

//rtos
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"





//C��
#include <string.h>
#include <stdio.h>

enum net_status{Conneted=0,Closed,Lost,Error};
enum net_status Net_status;
extern _Bool heart_flag;
volatile u8 secend;
extern u8 err_count;
extern uint8_t Ov7725_vsync;
extern _Bool photo;

unsigned int Task_Delay[NumOfTask]; 
extern OV7725_MODE_PARAM cam_mode;
char name[40];
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT res_sd;                /* �ļ�������� */
float frame_count = 0;
volatile data_Stream data_value;
u16 time_count=20;
static uint8_t name_count = 0;





///////////////////
TaskHandle_t NetTask_Handler;
TaskHandle_t ReceiveCmdTask_Handler;


//////////////////����Ҫ����ջ�ռ�
#define Net_Task_Stack 1500//500+
#define ReceiveCmd_Stack 256//54

/////////////////
#define Net_Task_Prioruty 6
#define ReceiveCMd_Priority 6



void NET_Event_CallBack(NET_EVENT net_event);



void Hardware_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������
  USART_Config();  //���Դ���
	LED_GPIO_Config();
	OV7725_GPIO_Config();
  //Usart3_Init(115200);  //WIFI���ڳ�ʼ��
  //ESP8266_Init();
	printf("wifi init \r\n");

	 M8266_init();

	
	
	
	//SysTick_Init();

	printf("Hardware init ok\r\n");
}






//�������
static void  CheckNetWork_Task(void **pvParameters)
{			  
	Net_status=(enum net_status)Check_Wifi(); //���wifiģ�������״̬


			if( Net_status!=Conneted)
			{
				if(Net_status==Lost)
	UsartPrintf(USART_DEBUG, "4. CIPSTART\r\n");
	while(ESP8266_SendCmd(CIPSTART, "CONNECT"))
		mDelay(500);
						while(OneNet_DevLink())			//����OneNET
	          mDelay(500);
				if(Net_status==Closed)
				{
					reLink();////��������
					printf("wifi is unusable");
			
				}
				
			}
}

static void Net_Task(void *pvParameters)
{
	unsigned short data_count=0,photo_count=0,check_count;
	
	while(1)
	{
	//printf("Net_Task\r\n");

		if(Net_status==Conneted)
		{

				 if(++data_count>=12)   //60s����һ������
		 {
			data_count=0;
			NET_Event_CallBack(NET_EVENT_Send_Data);
			}
	
		else if(photo==1||photo_count++>=time_count)
		{
			photo_count=0;

			
			taskENTER_CRITICAL(); //��ֹ�ж�
			NET_Event_CallBack(NET_EVENT_SendPhoto);
			photo=0;
			taskEXIT_CRITICAL();

				
		}
		else if(check_count>=20)
		{			
			NET_Event_CallBack(NET_EVENT_Check_Status);
		}
		
  }
	else
	{
	NET_Event_CallBack(NET_EVENT_Connect_Err);
	}
	vTaskDelay(1000/portTICK_RATE_MS);
	
}
}


 void OneNet_SendPhoto()
{
		name_count++;
			sprintf(name,"0:photo_%d.jpg",name_count); //�ַ�����ʽ��
			LED_BLUE;
			printf("\r\n��������...");	
	Ov7725_vsync=0;
		if(JPEG_encode(name)== 0)
			{
			//printf("\r\n���գ�����%s",name);
			OneNet_SendData_Picture(NULL,name);
			LED_GREEN;
			}
			else
			{
			printf("\r\n����ʧ��");
			LED_RED;
			}	
}

//�����·�������
static void ReceiveCmdTask(void *pvParameters) 
{
	while(1)
	{			
		OneNET_CmdHandle();
		NET_Event_CallBack(NET_EVENT_Recv);
		
	//	NET_Event_CallBack(NET_EVENT_Send_Data);//����ƽ̨����
		vTaskDelay(400/portTICK_RATE_MS);
	}
}


static void CheckSensor(void *pvParameters) 
{
	while(1)
	{			
		Check_sensor(&data_value);
		
	//	NET_Event_CallBack(NET_EVENT_Send_Data);//����ƽ̨����
		vTaskDelay(400/portTICK_RATE_MS);
	}
}


void NET_Event_CallBack(NET_EVENT net_event)
{

	switch((unsigned char)net_event)
	{
		case NET_EVENT_Send_HeartBeat:
		printf("SendHeart");
		OneNet_Send_heart();
		break;
		case NET_EVENT_SendPhoto:
		printf("SendPhoto\n");
		OneNet_SendPhoto();
		break;
		case NET_EVENT_Recv:
		break;	
   case NET_EVENT_Send_Data:
		printf("SendData\n");
		OneNet_SendData(&data_value);
		break;		
		case NET_EVENT_Connect_Err:
		printf("��������");
		reLink();
		break;
	}
}




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void) 	
{		
	u8 retry = 0;
	char name[40];
	Hardware_Init();
	
	
	/* Һ����ʼ�� */
while(OneNet_DevLink())			//����OneNET
		mDelay(500);
		Net_status=Conneted;

	/*����sd�ļ�ϵͳ*/
	res_sd = f_mount(&fs,"0:",1);
		
	if(res_sd != FR_OK)
	{
		printf("\r\n�����SD����\r\n");
	}

	/* ov7725 gpio ��ʼ�� */
	
	
	LED_BLUE;  
	
 if(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			printf("\r\nû�м�⵽OV7725����ͷ\r\n");
		}
	}
		printf("\r\nOV7725����ͷ��ʼ�����\r\n");


	/*��������ͷ����������ģʽ*/
	OV7725_Special_Effect(cam_mode.effect);
	/*����ģʽ*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*���Ͷ�*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*���ն�*/
	OV7725_Brightness(cam_mode.brightness);
	/*�Աȶ�*/
	OV7725_Contrast(cam_mode.contrast);
	/*����Ч��*/
	OV7725_Special_Effect(cam_mode.effect);
	
	/*����ͼ�������ģʽ��С*/
	OV7725_Window_Set(cam_mode.cam_sx,
														cam_mode.cam_sy,
														cam_mode.cam_width,
														cam_mode.cam_height,
														cam_mode.QVGA_VGA);
															Ov7725_vsync = 0;

/////////���濪ʼ�½�����
printf("start TASK\r\n");
//xTaskCreate((TaskFunction_t)ReceiveCmdTask,"ReceiveCmdTask",ReceiveCmd_Stack,"ReceiveCmdTask",ReceiveCMd_Priority,&ReceiveCmdTask_Handler);
xTaskCreate((TaskFunction_t)Net_Task,"Net_Task",Net_Task_Stack,"Net_Task",Net_Task_Prioruty,&NetTask_Handler);
vTaskStartScheduler();

}


	
	



/*********************************************END OF FILE**********************/

