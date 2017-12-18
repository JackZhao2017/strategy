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
#include "schedule_task.h"
#include "can_task.h"
#include "crc8.h"

FILTER filter[1]={{0x0cfe6c00,0x00ffff00,EXT}};

OS_STK START_TASK_STK[START_STK_SIZE];//�����ջ	
void start_task(void *pdata);					//������

void sys_switch(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
  /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ��*/	 
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);	                        											 
  /* Configure CAN pin: RX */	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PBout(12)=0;
}

int main()
{
		delay_init();	    	 	//��ʱ������ʼ��	
		sys_switch();
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
		LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
		uart_init(115200);
		crcInit(LSB,POLY);
		CAN_GPIO_Config();					//CAN�ܽų�ʼ��
	  CAN_NVIC_Configuration(); 	//CAN�жϳ�ʼ��   
		CAN_INIT(CAN_250kbps);
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
		can_task_init();
		OSTaskCreate(schedule_indicate_task,(void *)0,(OS_STK*)&SCHEDULE_INDICATE_TASK_STK[SCHEDULE_INDICATE_STK_SIZE-1],SCHEDULE_INDICATE_TASK_PRIO);
	  
		OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
		OS_EXIT_CRITICAL();							//�˳��ٽ���(���Ա��жϴ��)
}






