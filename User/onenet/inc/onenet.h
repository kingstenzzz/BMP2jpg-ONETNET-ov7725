#ifndef _ONENET_H_
#define _ONENET_H_



#define DEVID	"30964714"

#define APIKEY	"Uv=e=yMBymo8In9FVA4Ub16Oleo="
#define Heart_OK 1
#define Heart_err 0


typedef struct
{
	unsigned short time;
	unsigned   char temp;
	unsigned   char humidit;
	_Bool fire;
	unsigned char tem_max;
	unsigned char  tem_min;
	unsigned char hum_max;
	unsigned char hum_min;
	
	
	
	

} data_Stream;


typedef enum
{

	NET_EVENT_Timer_Check_Err = 0,			//���綨ʱ��鳬ʱ����
	NET_EVENT_Timer_Send_Err,				//���緢��ʧ�ܴ���
	
	NET_EVENT_Send_HeartBeat,				//��������������
	NET_EVENT_Send_Data,					//�����������ݵ�
	
	NET_EVENT_Recv,							//Modbus��-�յ����ݲ�ѯָ��
	NET_EVENT_SendPhoto,         //������Ƭ
	
	NET_EVENT_Check_Status,					//��������ģ��״̬���
	
	NET_EVENT_Device_Ok,					//����ģ����Ok
	NET_EVENT_Device_Err,					//����ģ�������
	
	NET_EVENT_Initialize,					//���ڳ�ʼ������ģ��
	NET_EVENT_Init_Ok,						//����ģ���ʼ���ɹ�
	
	NET_EVENT_Auto_Create_Ok,				//�Զ������豸�ɹ�
	NET_EVENT_Auto_Create_Err,				//�Զ������豸ʧ��
	
	NET_EVENT_Connect,						//�������ӡ���¼OneNET
	NET_EVENT_Connect_Ok,					//���ӡ���¼�ɹ�
	NET_EVENT_Connect_Err,					//���ӡ���¼����
	
	NET_EVENT_Fault_Process,				//���������

} NET_EVENT;



enum data_type{data_stream=0,picture,heart,Cmd} ;


_Bool OneNet_DevLink(void);

void OneNet_SendData(volatile data_Stream *data_stream);
void OneNet_RevPro(unsigned char *cmd);

//unsigned char EDP_UnPacketRecv(unsigned char *cmd);
void OneNet_SendData_Picture(char *devid,  char * pic_name);
void OneNet_Send_heart(void);
void OneNET_CmdHandle(void);

#endif
