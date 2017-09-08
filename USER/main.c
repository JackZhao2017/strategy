/*
	Created by Jack 2017/9/7
*/
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "taskconfig.h"
#include "can.h"
#include "usart.h"
#include "usart2.h"

FILTER filter[3]={{0x7e8,0x7ff,STD},{0x125,0x7ff,STD},{0x00fe6c00,0x00ffff00,EXT}};

OS_STK START_TASK_STK[START_STK_SIZE];//�����ջ	
void start_task(void *pdata);					//������

int main()
{
		delay_init();	    	 	//��ʱ������ʼ��	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
		LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
		uart_init(9600);
		CAN_GPIO_Config();					//CAN�ܽų�ʼ��
	  CAN_NVIC_Configuration(); 	//CAN�жϳ�ʼ��   
	  CAN_INIT(CAN_250kbps);			//CAN��ʼ��Nģ��	
		CAN_FILTERINIT(filter,sizeof(filter)/sizeof(FILTER));
		USART2_Init(115200);
		OSInit();   
		OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
		OSStart();	
}

void start_task(void *pdata)
{
		OS_CPU_SR cpu_sr=0;
		pdata = pdata; 
		delay_ms(1000);
		delay_ms(1000);
		OS_ENTER_CRITICAL();						//�����ٽ���(�޷����жϴ��)   
		printf("start_task\r\n");
		OSTaskCreate(schedule_indicate_task,(void *)0,(OS_STK*)&SCHEDULE_INDICATE_TASK_STK[SCHEDULE_INDICATE_STK_SIZE-1],SCHEDULE_INDICATE_TASK_PRIO);
		OSTaskCreate(obdii_task,(void *)0,(OS_STK*)&OBDII_TASK_STK[CAN_OBDII_STK_SIZE-1],CAN_OBDII_PRIO);	
		OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
		OS_EXIT_CRITICAL();							//�˳��ٽ���(���Ա��жϴ��)
}






