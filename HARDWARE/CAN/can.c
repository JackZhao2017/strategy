
#include "can.h"
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "can_speed.h"
#include "includes.h"
#include "taskconfig.h"
#include "can_task.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* ���жϴ������з��� */
__IO uint32_t ret = 0;

volatile TestStatus TestRx;	

/*CAN RX0 �ж����ȼ�����  */
 void CAN_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

  	/* Configure the NVIC Preemption Priority Bits */  
  
	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

	/* enabling interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/*CAN GPIO ��ʱ������ */
 void CAN_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ��*/	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	                        											 

  /* CAN1 ģ��ʱ��ʹ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 

	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
  /* Configure CAN pin: RX */	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11  ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1 ��ʵ��û���õ���ӳ��I/O
  // 
}

/*	CAN��ʼ�� */
 void CAN_INIT(int baudrate)
{
  CAN_InitTypeDef        CAN_InitStructure;

  /* CAN register init */
  CAN_DeInit(CAN1);	//������CAN��ȫ���Ĵ�������Ϊȱʡֵ
  CAN_StructInit(&CAN_InitStructure);//��CAN_InitStruct�е�ÿһ��������ȱʡֵ����

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM=DISABLE;//û��ʹ��ʱ�䴥��ģʽ
  CAN_InitStructure.CAN_ABOM=DISABLE;//û��ʹ���Զ����߹���
  CAN_InitStructure.CAN_AWUM=DISABLE;//û��ʹ���Զ�����ģʽ
  CAN_InitStructure.CAN_NART=DISABLE;//û��ʹ�ܷ��Զ��ش�ģʽ
  CAN_InitStructure.CAN_RFLM=DISABLE;//û��ʹ�ܽ���FIFO����ģʽ
  CAN_InitStructure.CAN_TXFP=DISABLE;//û��ʹ�ܷ���FIFO���ȼ�
  CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;//CAN����Ϊ����ģʽ
  CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //����ͬ����Ծ���1��ʱ�䵥λ
  CAN_InitStructure.CAN_BS1=CAN_BS1_3tq; //ʱ���1Ϊ3��ʱ�䵥λ
  CAN_InitStructure.CAN_BS2=CAN_BS2_2tq; //ʱ���2Ϊ2��ʱ�䵥λ
  CAN_InitStructure.CAN_Prescaler=baudrate;  //ʱ�䵥λ����Ϊ60	
  CAN_Init(CAN1,&CAN_InitStructure);
                                      //������Ϊ��72M/2/60(1+3+2)=0.1 ��100K
  /* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); //ʹ��FIFO0��Ϣ�Һ��ж�
 
 }  

 
void CAN_FILTERINIT(FILTER *filter,int len)
{
		int i=0;
		unsigned int filterId;
		unsigned int filterMaskId;
		CAN_FilterInitTypeDef  CAN_FilterInitStructure; 
	  /* CAN filter init */
		for(i=0;i<len;i++){
			filterId=filter[i].CAN_ID<<(32-filter[i].CAN_ID_FMT);
			filterMaskId=filter[i].CAN_ID_MASK<<(32-filter[i].CAN_ID_FMT);
			CAN_FilterInitStructure.CAN_FilterNumber=i;//ָ��������Ϊ1
			CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//ָ��������Ϊ��ʶ������λģʽ
			CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//������λ��Ϊ32λ
			CAN_FilterInitStructure.CAN_FilterIdHigh=(filterId>>16)&0xffff;// ��������ʶ���ĸ�16λֵ
			if(filter[i].CAN_ID_FMT==EXT){
					CAN_FilterInitStructure.CAN_FilterIdLow=(filterId&0xffff)|CAN_ID_EXT;//	 ��������ʶ���ĵ�16λֵ
			}else{
					CAN_FilterInitStructure.CAN_FilterIdLow=filterId&0xffff;
			}
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(filterMaskId>>16)&0xffff;//���������α�ʶ���ĸ�16λֵ
			CAN_FilterInitStructure.CAN_FilterMaskIdLow=(filterMaskId&0xffff)|0x6;//	���������α�ʶ���ĵ�16λֵ
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;// �趨��ָ���������FIFOΪ0
			CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;// ʹ�ܹ�����
			CAN_FilterInit(&CAN_FilterInitStructure);//	������Ĳ�����ʼ��������
			printf("idh 	=0x%4x idl 	= 0x%04x\r\n",CAN_FilterInitStructure.CAN_FilterIdHigh,CAN_FilterInitStructure.CAN_FilterIdLow);
			printf("idMh 	=0x%4x idMl = 0x%04x\r\n",CAN_FilterInitStructure.CAN_FilterMaskIdHigh,CAN_FilterInitStructure.CAN_FilterMaskIdLow);
		}
}

void can_tx(u32 id ,u32 id_fmt,u8 *data,u8 len)
{ 
		CanTxMsg TxMessage;  

		TxMessage.StdId=id;	//��׼��ʶ��Ϊ0x00
		TxMessage.ExtId=id; //��չ��ʶ��0x0000
		TxMessage.IDE=id_fmt;//ʹ�ñ�׼��ʶ��
		TxMessage.RTR=CAN_RTR_DATA;//Ϊ����֡
		TxMessage.DLC=len;	//	��Ϣ�����ݳ���Ϊ2���ֽ�
		memcpy(TxMessage.Data,data,len);
		CAN_Transmit(CAN1,&TxMessage); //�������� 
}
/* USB�жϺ�CAN�����жϷ������USB��CAN����I/O������ֻ�õ�CAN���жϡ� */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
		OS_CPU_SR cpu_sr=0;	
		CanRxMsg RxMessage;
	
		OS_ENTER_CRITICAL();
	
		memset(&RxMessage,0,sizeof(CanRxMsg));  
		CAN_Receive(CAN1,CAN_FIFO0, &RxMessage); //����FIFO0�е�����  
		resolve_can_message(&RxMessage);	
		OS_EXIT_CRITICAL();				
} 


