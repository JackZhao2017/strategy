#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "includes.h"
/////////////////////////UCOSII��������///////////////////////////////////

//START ����


#define SEND_TASK_PRIO       			5 	//�����������ȼ�
#define SEND_STK_SIZE  		    		64 	//���������ջ��С


//LED����
#define SCHEDULE_INDICATE_TASK_PRIO       			9 	//�����������ȼ�
#define SCHEDULE_INDICATE_STK_SIZE  		    		64 	//���������ջ��С


void speed_send_task(OS_TMR *ptmr, void *pdata);

void schedule_indicate_task(void *pdata);        	//������
extern OS_STK SCHEDULE_INDICATE_TASK_STK[SCHEDULE_INDICATE_STK_SIZE];	//�����ջ	
void set_system_status(s8 status);

void set_canbus_curmode(u8 mode);

void set_canbus_datamode(u8 mode);

#endif
