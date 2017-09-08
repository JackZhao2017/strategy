#ifndef _TASKCONFIG_H_
#define _TASKCONFIG_H_

#include "includes.h"
/////////////////////////UCOSII��������///////////////////////////////////

//START ����

#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
#define START_STK_SIZE  					64  //���������ջ��С


#define SEND_TASK_PRIO       			5 	//�����������ȼ�
#define SEND_STK_SIZE  		    		64 	//���������ջ��С


//LED����
#define SCHEDULE_INDICATE_TASK_PRIO       			7 	//�����������ȼ�
#define SCHEDULE_INDICATE_STK_SIZE  		    		64 	//���������ջ��С

#define CAN_OBDII_PRIO						6  
#define CAN_OBDII_STK_SIZE				256

#define TASK_ON   1
#define TASK_OFF  0

extern OS_STK OBDII_TASK_STK[CAN_OBDII_STK_SIZE];
void obdii_task(void *pdata);
void resolve_obdii_speed(u8 *data);
u8 get_obdii_task_status(void);
s8 set_obdii_task_status(u8 status);


void speed_send_task(OS_TMR *ptmr, void *pdata);

void schedule_indicate_task(void *pdata);        	//������
extern OS_STK SCHEDULE_INDICATE_TASK_STK[SCHEDULE_INDICATE_STK_SIZE];	//�����ջ	
void set_system_status(s8 status);
#endif
