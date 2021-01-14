/*********************************************************************************
 * �ļ���  ��usart.c
 * ����    ��1. ��c���е�printf()����ʵ�ֵ�����2(USART3)���������ǾͿ�����printf()��
 *              ������Ϣͨ�����ڴ�ӡ�������ϡ� 
 *           2. �����жϽ��ܺ��� �����ܵ����ַ����ŵ� USART_RX_BUF �ַ�����
 *         	    USART_RX_STA��ʾ�����ַ����ĳ���
 *           3.USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
 *             USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
 *           4.ͨ������EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
 *              EN_USART3_RX�� usart.h����ʹ�ܶ���
 *           5.ͨ���ж�USART_RX_STA �Ƿ����x8000��ȷ���Ƿ�������
 *             ͨ����ȡ�ַ���USART_RX_BUF[]��ֵ����ȡ�յ�������
 * �മ��  ��1.��������ڲ���ʱ����һ�����ڿ�����printf
 *           2.��ô����USART_printf()��������printf�Ĺ����������˴���ѡ��  
 *           3.��usart.h����������ͷ�ļ�<stdarg.h>  �ͱ��뿪��USART_printf_EN 
 *             ��Ҫ��USART_printf()ʱ ����Ϊ1 ����ʱ����Ϊ0 
 * Ӳ�����ӣ�------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *          | PA2  - USART2(Tx)      |
 *          | PA3  - USART2(Rx)      |
 *          | PB10 - USART3(Tx)      |
 *          | PB11 - USART3(Rx)      |
 *          | PC10 - UART4(Tx)       |
 *          | PC11 - UART4(Rx)       |
 *          | PC12 - UART5(Tx)       |
 *          | PD2  - UART5(Rx)       |
 *           ------------------------ 
 * ��ֲ˵����ֻҪ��USART1.c �� usrart1.h��ӵ�����Ҫ�Ĺ����� ͨ��printf����ӡ������Ϣ
 *           ��ȡUSART_RX_BUF�ַ�������������ȡ���� USART_RX_STA ��ȡ�����ַ����ĳ���
 * ����    ��2014 08 06 2019 07 30
**********************************************************************************/
#include "sys.h"
#include "usart.h"	  
#include "delay.h"
#include "GPIO.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 

                          
u8 USART1_RX_BUF[128];  	//���ջ���,���128���ֽ�. //���ջ����� 	                          
u8 USART1_RX_CNT=0;      	//���յ������ݳ���
u8 USART2_RX_BUF[128];  	//���ջ���,���128���ֽ�. //���ջ����� 	                          
u8 USART2_RX_CNT=0;      	//���յ������ݳ���
u8 USART3_RX_BUF[128];  	//���ջ���,���128���ֽ�. //���ջ����� 	                          
u8 USART3_RX_CNT=0;      	//���յ������ݳ���
u8 UART4_RX_BUF[128];  		//���ջ���,���128���ֽ�. //���ջ����� 	                          
u8 UART4_RX_CNT=0;      	//���յ������ݳ���
u8 UART5_RX_BUF[128];  		//���ջ���,���128���ֽ�. //���ջ����� 	                          
u8 UART5_RX_CNT=0;      	//���յ������ݳ���

/********************************************************************************************/
/******************************************USART1********************************************/
/********************************************************************************************/
/**************************************************************
 * ��������USART1_IRQHandler
 * ����  ������1
 * ����  ����
 * ���  : ��
 **************************************************************/ 
void USART1_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���յ�����
	{	 		 
		res =USART_ReceiveData(USART1); 	                   //��ȡ���յ�������
		
		if(USART1_RX_CNT<128)
		{
			USART1_RX_BUF[USART1_RX_CNT]=res;		                //��¼���յ���ֵ
			USART1_RX_CNT++;						                        //������������1 
		} 
	}  											 
} 
/**************************************************************
 * ��������USART1_Receive_Data
 * ����  ��USART1��ѯ���յ�������
 * ����  ��buf:���ջ����׵�ַ
 *         len:���������ݳ���
 * ���  : ��
 **************************************************************/
void USART1_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART1_RX_CNT;
	u8 i=0;
	*len=0;				                 		//Ĭ��Ϊ0
	delay_ms(10);		               		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART1_RX_CNT&&rxlen) 	//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART1_RX_BUF[i];	
		}		
		*len=USART1_RX_CNT;	         		//��¼�������ݳ���
		USART1_RX_CNT=0;		          	//����
	}	
}
/**************************************************************
 * ��������USART1_Send_Data
 * ����  ��USART1����len���ֽ�.
 * ����  ��buf:�������׵�ַ
 *         len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 * ���  : ��
 **************************************************************/
void USART1_Send_Data(u8* buf,u8 len)
{
	u8 t;
  for(t=0;t<len;t++)		        		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1,buf[t]);
	}	 
 	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		  	
}



/********************************************************************************************/
/******************************************USART2********************************************/
/********************************************************************************************/
/**************************************************************
 * ��������USART2_Send_Data
 * ����  ��USART2����len���ֽ�.
 * ����  ��buf:�������׵�ַ
 *         len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 * ���  : ��
 **************************************************************/
void USART2_Send_Data(u8* buf,u8 len)
{
	u8 t;
  for(t=0;t<len;t++)		        //ѭ����������
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
 	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		  	
}
/**************************************************************
 * ��������USART2_IRQHandler
 * ����  ������2
 * ����  ����
 * ���  : ��
 **************************************************************/ 

void USART2_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //���յ�����
	{	 		 
		res =USART_ReceiveData(USART2); 	                   //��ȡ���յ�������
		
		if(USART2_RX_CNT<128)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;		                 //��¼���յ���ֵ
			USART2_RX_CNT++;						                         //������������1 
		} 
	}  											 
} 
/**************************************************************
 * ��������USART2_Receive_Data
 * ����  ��USART2��ѯ���յ�������
 * ����  ��buf:���ջ����׵�ַ
 *         len:���������ݳ���
 * ���  : ��
 **************************************************************/
void USART2_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART2_RX_CNT;
	u8 i=0;
	*len=0;				                 //Ĭ��Ϊ0
	delay_ms(10);		               //�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART2_RX_CNT&&rxlen) //���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	
		}		
		*len=USART2_RX_CNT;	         //��¼�������ݳ���
		USART2_RX_CNT=0;		           //����
	}	
}




/********************************************************************************************/
/******************************************USART3********************************************/
/********************************************************************************************/
/**************************************************************
 * ��������USART3_IRQHandler
 * ����  ������3
 * ����  ����
 * ���  : ��
 **************************************************************/ 

void USART3_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  	 //���յ�����
	{	 		 
		res =USART_ReceiveData(USART3); 	                  	 //��ȡ���յ�������
		
		if(USART3_RX_CNT<128)
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;		                 //��¼���յ���ֵ
			USART3_RX_CNT++;						                         //������������1 
		} 
	}  											 
} 

/**************************************************************
 * ��������USART3_Receive_Data
 * ����  ��USART3��ѯ���յ�������
 * ����  ��buf:���ջ����׵�ַ
 *         len:���������ݳ���
 * ���  : ��
 **************************************************************/
void USART3_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART3_RX_CNT;
	u8 i=0;
	*len=0;				                 //Ĭ��Ϊ0
	delay_ms(10);		               //�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART3_RX_CNT&&rxlen) //���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	
		}	
		*len=USART3_RX_CNT;	         //��¼�������ݳ���
		USART3_RX_CNT=0;		           //����
	}	
}


/**************************************************************
 * ��������USART3_Send_Data
 * ����  ��USART3����len���ֽ�.
 * ����  ��buf:�������׵�ַ
 *         len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 * ���  : ��
 **************************************************************/
void USART3_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //ѭ����������
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		  

}


/********************************************************************************************/
/******************************************USART4********************************************/
/********************************************************************************************/
/**************************************************************
 * ��������UART4_IRQHandler
 * ����  ������4
 * ����  ����
 * ���  : ��
 **************************************************************/ 
void UART4_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //���յ�����
	{	 		 
		res =USART_ReceiveData(UART4); 	                   //��ȡ���յ�������
		
		if(UART4_RX_CNT<128)
		{
			UART4_RX_BUF[UART4_RX_CNT]=res;		                 //��¼���յ���ֵ
			UART4_RX_CNT++;						                         //������������1 
		} 
	}  											 
} 
/**************************************************************
 * ��������UART4_Receive_Data
 * ����  ��UART4��ѯ���յ�������
 * ����  ��buf:���ջ����׵�ַ
 *         len:���������ݳ���
 * ���  : ��
 **************************************************************/
void UART4_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=UART4_RX_CNT;
	u8 i=0;
	*len=0;				                 //Ĭ��Ϊ0
	delay_ms(10);		               //�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==UART4_RX_CNT&&rxlen) //���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=UART4_RX_BUF[i];	
		}		
		*len=UART4_RX_CNT;	         //��¼�������ݳ���
		UART4_RX_CNT=0;		           //����
	}	
}


/**************************************************************
 * ��������UART4_Send_Data
 * ����  ��UART4����len���ֽ�.
 * ����  ��buf:�������׵�ַ
 *         len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 * ���  : ��
 **************************************************************/
void UART4_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //ѭ����������
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4,buf[t]);
	}	 
 	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);		  	
}


/********************************************************************************************/
/******************************************USART5********************************************/
/********************************************************************************************/
/**************************************************************
 * ��������UART5_IRQHandler
 * ����  ������5
 * ����  ����
 * ���  : ��
 **************************************************************/ 
void UART5_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //���յ�����
	{	 		 
		res =USART_ReceiveData(UART5); 	                   //��ȡ���յ�������
		
		if(UART5_RX_CNT<128)
		{
			UART5_RX_BUF[UART5_RX_CNT]=res;		                 //��¼���յ���ֵ
			UART5_RX_CNT++;						                         //������������1 
		} 
	}  											 
} 
/**************************************************************
 * ��������UART5_Receive_Data
 * ����  ��UART5��ѯ���յ�������
 * ����  ��buf:���ջ����׵�ַ
 *         len:���������ݳ���
 * ���  : ��
 **************************************************************/
void UART5_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=UART5_RX_CNT;
	u8 i=0;
	*len=0;				                 //Ĭ��Ϊ0
	delay_ms(10);		               //�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==UART5_RX_CNT&&rxlen) //���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=UART5_RX_BUF[i];	
		}		
		*len=UART5_RX_CNT;	         //��¼�������ݳ���
		UART5_RX_CNT=0;		           //����
	}	
}


/**************************************************************
 * ��������UART5_Send_Data
 * ����  ��UART5����len���ֽ�.
 * ����  ��buf:�������׵�ַ
 *         len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 * ���  : ��
 **************************************************************/
void UART5_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //ѭ����������
	{		   
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART5,buf[t]);
	}	 
 	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);		  	
}


/*****************************************************
 * ��������uart_init
 * ����  �����ڳ�ʼ������
 * Ӳ��  ��TX4--->PC10 RX4--->PC11
 *         TX5--->PC12 RX5--->PD2
 * ����  ��������
 * ���  ����
 ****************************************************/
void uart_init(u32 bound1,u32 bound2,u32 bound3,u32 bound4,u32 bound5)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//uart1-3 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB1Periph_USART2, ENABLE);
	//uart4-5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4|RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
	//��λ����
 	USART_DeInit(USART1);  
  USART_DeInit(USART2);
	USART_DeInit(USART3);
	USART_DeInit(UART4);
	USART_DeInit(UART5);
	
	
  //����1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
	//����2	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//����3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//����4 TX4--->PC10 RX4--->PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//����5 TX5--->PC12 RX5--->PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure); 




  //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound1;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������
	
	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound2;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������
	
	//USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound3;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������
	
	//UART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound4;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������
 
 	//UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound5;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������
 
 
 
 
 //USART1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	
	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	
  //USART3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	  //UART4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�
	  //UART5 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�


  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 1
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 2
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 3
	 
	USART_Cmd(UART4, ENABLE);                    	//ʹ�ܴ��� 4
  USART_Cmd(UART5, ENABLE);                    	//ʹ�ܴ��� 5

}

#if USART_printf_EN 
/*****************************************************
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART_printf()����
 ****************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} 

/*****************************************************
 * ��������USART_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ����USART1��USART3��USART3
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART_printf( USART3, "\r\n this is a demo \r\n" );
 *            		 USART_printf( USART3, "\r\n %d \r\n", i );
 *            		 USART_printf( USART3, "\r\n %s \r\n", j )
 ****************************************************/
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
 		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
#endif
