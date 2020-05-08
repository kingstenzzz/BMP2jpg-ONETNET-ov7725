

//��Ƭ��ͷ�ļ�


//�����豸
#include "esp8266.h"

#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"

//Э���ļ�
#include "onenet.h"
#include "edpkit.h"
#include "ff.h"
#include "encode.h"



//Ӳ������
#include "./usart/bsp_usart.h"
#include "delay.h"
#include "iic.h"
#include "timer.h"
#include "./key/bsp_key.h"  



//C��
#include <string.h>
#include <stdio.h>

//////////cmd��main����δ��




extern unsigned char esp8266_buf[1024];
_Bool  heart_flag;
extern enum data_type Data_type;
_Bool photo=0;
extern   u16 time_count;
extern u16 link_status;
extern u8 link_no;
unsigned char camera_buf[1000];
FIL bmpfsrc; 
FRESULT bmpres;

 int16_t temp_max=100;
 int16_t temp_min=10;
 u8 humi_max=100;
 u8 humi_min=0;
 _Bool fire;
 extern uint8_t Ov7725_vsync;




//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		//ESP8266_SendData(edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
	
		dataPtr = M8266_GetIPD(250);								//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��豸ID��Ȩʧ��\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û�ID��Ȩʧ��\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�δ��Ȩ\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ���Ȩ����Ч\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������δ����\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ����豸�ѱ�����\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��ظ��������������\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf,data_Stream *data_stream)
{
	char text[16];
	memset(text, 0, sizeof(text));
	strcpy(buf, "{");
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Fire\":%d,",data_stream->fire);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temperature\":%d,",data_stream->temp);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Humidity\":%d,",data_stream->humidit);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temp_Max\":%d,",data_stream->tem_max);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temp_Min\":%d,",data_stream->tem_min);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Hum_Max\":%d,",data_stream->hum_max);///////ע�ⶺ��
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Hum_Min\":%d",data_stream->hum_min);
	strcat(buf, text);
	strcat(buf, "}");	
	return strlen(buf);
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(volatile data_Stream *data_stream)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short int body_len = 0, i = 0;
		
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf,data_stream);		//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
										//�ϴ����ݵ�ƽ̨
			EDP_DeleteBuffer(&edpPacket);
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	//ESP8266_Clear();
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================



void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	//Э���
	
	char *cmdid_devid = NULL;
	unsigned short cmdid_len = 0;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	heart_flag=Heart_err;//�ȳ�ʼ��δ��ʧ
	hexdump(edpPacket._data,edpPacket._len);//��ӡ�յ�����Ϣ
	type = EDP_UnPacketRecv(cmd);
	switch(type)										//�ж���pushdata���������·�
	{
			case PUSHDATA:									//��pushdata��
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		case CMDREQ:									//�������
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//����ɹ������������ظ������
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			}
			
		break;
			
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				//UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
		break;
		case PINGRESP:
			printf("������Ӧ");
			heart_flag=Heart_OK;
		break;
				
			
		default:
			result = -1;
		break;
	}
	
		//M8266_Clear();
			
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req, ':');							//����':'
	
	if(dataPtr != NULL)									//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		if(strstr((char *)req, "photo"))				//����"Fire"
		{
			
			if(num == 1)       photo=1;
			else             	 photo=0;	
	
		}
				else if(strstr((char *)req, "time"))				//����"Fire"
				{
					time_count=num;   //��ֵ

				}
					else if(strstr((char *)req, "Hum_Max"))				//����"Fire"
				{
					humi_max=num;   //��ֵ

				}
					else if(strstr((char *)req, "Hum_Min"))				//����"Fire"
				{
					humi_min=num;   //��ֵ

				}
						else if(strstr((char *)req, "Temp_Max"))				//����"Fire"
				{
					temp_max=num;   //��ֵ

				}
						else if(strstr((char *)req, "Temp_Min"))				//����"Fire"
				{
					temp_min=num;   //��ֵ

				}
				else if(strstr((char *)req, "door"))
				{
					if(num == 1)       
					{Door_OPEN;
					 mDelay(3000);
					Door_ClOSE;
					}
			    else             	 Door_ClOSE;	
					
				}
				else if(strstr((char *)req, "Door"))
				{
					if(num == 1)       
					{
					photo=1;
					printf("���˿���\r\n");

					}
				}
	}
	
	if(type == CMDREQ && result == 0)						//���������� �� ����ɹ�
	{
		EDP_FreeBuffer(cmdid_devid);						//�ͷ��ڴ�
		EDP_FreeBuffer(req);												//�ظ�����
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);	//�ϴ�ƽ̨
		EDP_DeleteBuffer(&edpPacket);						//ɾ��
	}

}



void OneNet_Send_heart(void)
{
		EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	
		if(!EDP_PacketPing(&edpPacket))
		{
		printf("send ping pkt to server, bytes: %d\r\n", edpPacket._len);
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
			hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//ɾ��
		}
		else 
		printf("����ʧ��");
}





#define PKT_SIZE 1000
void OneNet_SendData_Picture(char *devid,  char * pic_name)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};					//Э���
	
	char type_bin_head[] = "{\"ds_id\":\"ppp\"}";						//ͼƬ����ͷ
	unsigned int read_num;	u16 pic_len;
	u16 ucAlign;

	bmpres = f_open( &bmpfsrc , (char *)pic_name, FA_OPEN_EXISTING | FA_READ);	
/*-------------------------------------------------------------------------------------------------------*/
	if(bmpres == FR_OK)
	{
		printf("���ļ��ɹ�\r\n");

		/* ��ȡ�ļ�ͷ��Ϣ  �����ֽ�*/  
		pic_len=bmpfsrc.fsize;
		ucAlign=pic_len%PKT_SIZE;
	
	if(EDP_PacketSaveData(devid, pic_len, type_bin_head, kTypeBin, &edpPacket) == 0)
	{	
			M8266_Clear();
			M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
		//hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//ɾ��
		UsartPrintf(USART_DEBUG, "image len = %d\r\n", pic_len);
		while(pic_len > 0)
		{
			mDelay(500);												//��ͼʱ��ʱ�������һ�㣬�����������һ����ʱ	
			if(pic_len >= PKT_SIZE)
			{
			bmpres=f_read(&bmpfsrc,&camera_buf,PKT_SIZE*sizeof(unsigned char),&read_num);
			//if(read_num||res==0) break;
			M8266WIFI_SPI_Send_Data(camera_buf,PKT_SIZE,link_no,&link_status);
				//���ڷ��ͷ�Ƭ
				
		//		pImage += PKT_SIZE;
				pic_len -= PKT_SIZE;
			}
			
			else
			{
			
				bmpres=f_read(&bmpfsrc,&camera_buf,ucAlign*sizeof(unsigned char),&read_num);
				M8266WIFI_SPI_Send_Data(camera_buf,ucAlign,link_no,&link_status);
				//ESP8266_SendData(camera_buf, ucAlign);					//���ڷ������һ����Ƭ
				pic_len = 0;
			}
		}
		
		UsartPrintf(USART_DEBUG, "image send ok\r\n");
	}
	else
		UsartPrintf(USART_DEBUG, "EDP_PacketSaveData Failed\r\n");

	

}
	
	}


//==========================================================
//	�������ƣ�	OneNET_CmdHandle
//
//	�������ܣ�	��ȡƽ̨rb�е�����
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
unsigned char  OneNET_CmdHandle(void)
{
	
	unsigned char *dataPtr = NULL;		//����ָ��
	
	dataPtr = M8266_GetIPD(5);		//����Ƿ���ƽ̨����
		if(dataPtr != NULL)
		{	
				OneNet_RevPro(dataPtr);					//���д���
			  return 1;
			
		}
		return 0;

}




 u8 OneNet_SendPhoto(void)
{
		static int name_count=0;
	  char name[40];
		name_count++;
		sprintf(name,"0:photos_%d.jpg",name_count); //�ַ�����ʽ��
		printf("\r\n��������...");	
	  Ov7725_vsync=0;
		if(JPEG_encode(name)== 0)
			{
			  OneNet_SendData_Picture(NULL,name);
				return 1;	
			}
			else
			{
			  printf("\r\n����ʧ��");
				return 0;
				
				
			}	
}

