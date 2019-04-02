#ifndef _BEEP_H_
#define _BEEP_H_


#include "stm32f10x.h"


/************************** BEEP �������Ͷ���********************************/
typedef struct
{
	_Bool beep_status;
}BEEP_INFO;


/************************** BEEP �����궨��********************************/
#define BEEP_ON		1
#define BEEP_OFF	0


/************************** BEEP �ṹ�嶨��********************************/
extern BEEP_INFO beepInfo;


/************************** BEEP �����궨��********************************/
void Beep_Init(void);
void Beep_Set(_Bool status);


#endif
