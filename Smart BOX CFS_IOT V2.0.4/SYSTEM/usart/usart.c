/*********************************************************************************
 * 文件名  ：usart.c
 * 描述    ：1. 将c库中的printf()函数实现到串口2(USART3)。这样我们就可以用printf()将
 *              调试信息通过串口打印到电脑上。 
 *           2. 串口中断接受函数 将接受到的字符串放到 USART_RX_BUF 字符串中
 *         	    USART_RX_STA表示接收字符串的长度
 *           3.USART_RX_STA,使得串口最大接收字节数为2的14次方
 *             USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
 *           4.通过设置EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
 *              EN_USART3_RX在 usart.h里面使能定义
 *           5.通过判断USART_RX_STA 是否等于x8000来确定是否接受完成
 *             通过读取字符串USART_RX_BUF[]的值来读取收到的数据
 * 多串口  ：1.当多个串口并用时就有一个串口可以用printf
 *           2.怎么加了USART_printf()函数，在printf的功能上增加了串口选择  
 *           3.在usart.h里面增加了头文件<stdarg.h>  和编译开关USART_printf_EN 
 *             想要用USART_printf()时 设置为1 不用时设置为0 
 * 硬件连接：------------------------
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
 * 移植说明：只要把USART1.c 和 usrart1.h添加到所需要的工程里 通过printf来打印串口信息
 *           读取USART_RX_BUF字符串的内容来读取接受 USART_RX_STA 读取接受字符串的长度
 * 日期    ：2014 08 06 2019 07 30
**********************************************************************************/
#include "sys.h"
#include "usart.h"	  
#include "delay.h"
#include "GPIO.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 

                          
u8 USART1_RX_BUF[128];  	//接收缓冲,最大128个字节. //接收缓存区 	                          
u8 USART1_RX_CNT=0;      	//接收到的数据长度
u8 USART2_RX_BUF[128];  	//接收缓冲,最大128个字节. //接收缓存区 	                          
u8 USART2_RX_CNT=0;      	//接收到的数据长度
u8 USART3_RX_BUF[128];  	//接收缓冲,最大128个字节. //接收缓存区 	                          
u8 USART3_RX_CNT=0;      	//接收到的数据长度
u8 UART4_RX_BUF[128];  		//接收缓冲,最大128个字节. //接收缓存区 	                          
u8 UART4_RX_CNT=0;      	//接收到的数据长度
u8 UART5_RX_BUF[128];  		//接收缓冲,最大128个字节. //接收缓存区 	                          
u8 UART5_RX_CNT=0;      	//接收到的数据长度

/********************************************************************************************/
/******************************************USART1********************************************/
/********************************************************************************************/
/**************************************************************
 * 函数名：USART1_IRQHandler
 * 描述  ：串口1
 * 输入  ：无
 * 输出  : 无
 **************************************************************/ 
void USART1_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收到数据
	{	 		 
		res =USART_ReceiveData(USART1); 	                   //读取接收到的数据
		
		if(USART1_RX_CNT<128)
		{
			USART1_RX_BUF[USART1_RX_CNT]=res;		                //记录接收到的值
			USART1_RX_CNT++;						                        //接收数据增加1 
		} 
	}  											 
} 
/**************************************************************
 * 函数名：USART1_Receive_Data
 * 描述  ：USART1查询接收到的数据
 * 输入  ：buf:接收缓存首地址
 *         len:读到的数据长度
 * 输出  : 无
 **************************************************************/
void USART1_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART1_RX_CNT;
	u8 i=0;
	*len=0;				                 		//默认为0
	delay_ms(10);		               		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USART1_RX_CNT&&rxlen) 	//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART1_RX_BUF[i];	
		}		
		*len=USART1_RX_CNT;	         		//记录本次数据长度
		USART1_RX_CNT=0;		          	//清零
	}	
}
/**************************************************************
 * 函数名：USART1_Send_Data
 * 描述  ：USART1发送len个字节.
 * 输入  ：buf:发送区首地址
 *         len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 * 输出  : 无
 **************************************************************/
void USART1_Send_Data(u8* buf,u8 len)
{
	u8 t;
  for(t=0;t<len;t++)		        		//循环发送数据
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
 * 函数名：USART2_Send_Data
 * 描述  ：USART2发送len个字节.
 * 输入  ：buf:发送区首地址
 *         len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 * 输出  : 无
 **************************************************************/
void USART2_Send_Data(u8* buf,u8 len)
{
	u8 t;
  for(t=0;t<len;t++)		        //循环发送数据
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
 	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		  	
}
/**************************************************************
 * 函数名：USART2_IRQHandler
 * 描述  ：串口2
 * 输入  ：无
 * 输出  : 无
 **************************************************************/ 

void USART2_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收到数据
	{	 		 
		res =USART_ReceiveData(USART2); 	                   //读取接收到的数据
		
		if(USART2_RX_CNT<128)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;		                 //记录接收到的值
			USART2_RX_CNT++;						                         //接收数据增加1 
		} 
	}  											 
} 
/**************************************************************
 * 函数名：USART2_Receive_Data
 * 描述  ：USART2查询接收到的数据
 * 输入  ：buf:接收缓存首地址
 *         len:读到的数据长度
 * 输出  : 无
 **************************************************************/
void USART2_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART2_RX_CNT;
	u8 i=0;
	*len=0;				                 //默认为0
	delay_ms(10);		               //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USART2_RX_CNT&&rxlen) //接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	
		}		
		*len=USART2_RX_CNT;	         //记录本次数据长度
		USART2_RX_CNT=0;		           //清零
	}	
}




/********************************************************************************************/
/******************************************USART3********************************************/
/********************************************************************************************/
/**************************************************************
 * 函数名：USART3_IRQHandler
 * 描述  ：串口3
 * 输入  ：无
 * 输出  : 无
 **************************************************************/ 

void USART3_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  	 //接收到数据
	{	 		 
		res =USART_ReceiveData(USART3); 	                  	 //读取接收到的数据
		
		if(USART3_RX_CNT<128)
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;		                 //记录接收到的值
			USART3_RX_CNT++;						                         //接收数据增加1 
		} 
	}  											 
} 

/**************************************************************
 * 函数名：USART3_Receive_Data
 * 描述  ：USART3查询接收到的数据
 * 输入  ：buf:接收缓存首地址
 *         len:读到的数据长度
 * 输出  : 无
 **************************************************************/
void USART3_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USART3_RX_CNT;
	u8 i=0;
	*len=0;				                 //默认为0
	delay_ms(10);		               //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USART3_RX_CNT&&rxlen) //接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	
		}	
		*len=USART3_RX_CNT;	         //记录本次数据长度
		USART3_RX_CNT=0;		           //清零
	}	
}


/**************************************************************
 * 函数名：USART3_Send_Data
 * 描述  ：USART3发送len个字节.
 * 输入  ：buf:发送区首地址
 *         len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 * 输出  : 无
 **************************************************************/
void USART3_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //循环发送数据
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
 * 函数名：UART4_IRQHandler
 * 描述  ：串口4
 * 输入  ：无
 * 输出  : 无
 **************************************************************/ 
void UART4_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收到数据
	{	 		 
		res =USART_ReceiveData(UART4); 	                   //读取接收到的数据
		
		if(UART4_RX_CNT<128)
		{
			UART4_RX_BUF[UART4_RX_CNT]=res;		                 //记录接收到的值
			UART4_RX_CNT++;						                         //接收数据增加1 
		} 
	}  											 
} 
/**************************************************************
 * 函数名：UART4_Receive_Data
 * 描述  ：UART4查询接收到的数据
 * 输入  ：buf:接收缓存首地址
 *         len:读到的数据长度
 * 输出  : 无
 **************************************************************/
void UART4_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=UART4_RX_CNT;
	u8 i=0;
	*len=0;				                 //默认为0
	delay_ms(10);		               //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==UART4_RX_CNT&&rxlen) //接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=UART4_RX_BUF[i];	
		}		
		*len=UART4_RX_CNT;	         //记录本次数据长度
		UART4_RX_CNT=0;		           //清零
	}	
}


/**************************************************************
 * 函数名：UART4_Send_Data
 * 描述  ：UART4发送len个字节.
 * 输入  ：buf:发送区首地址
 *         len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 * 输出  : 无
 **************************************************************/
void UART4_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //循环发送数据
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
 * 函数名：UART5_IRQHandler
 * 描述  ：串口5
 * 输入  ：无
 * 输出  : 无
 **************************************************************/ 
void UART5_IRQHandler(void)
{
	u8 res;	    
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收到数据
	{	 		 
		res =USART_ReceiveData(UART5); 	                   //读取接收到的数据
		
		if(UART5_RX_CNT<128)
		{
			UART5_RX_BUF[UART5_RX_CNT]=res;		                 //记录接收到的值
			UART5_RX_CNT++;						                         //接收数据增加1 
		} 
	}  											 
} 
/**************************************************************
 * 函数名：UART5_Receive_Data
 * 描述  ：UART5查询接收到的数据
 * 输入  ：buf:接收缓存首地址
 *         len:读到的数据长度
 * 输出  : 无
 **************************************************************/
void UART5_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=UART5_RX_CNT;
	u8 i=0;
	*len=0;				                 //默认为0
	delay_ms(10);		               //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==UART5_RX_CNT&&rxlen) //接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=UART5_RX_BUF[i];	
		}		
		*len=UART5_RX_CNT;	         //记录本次数据长度
		UART5_RX_CNT=0;		           //清零
	}	
}


/**************************************************************
 * 函数名：UART5_Send_Data
 * 描述  ：UART5发送len个字节.
 * 输入  ：buf:发送区首地址
 *         len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 * 输出  : 无
 **************************************************************/
void UART5_Send_Data(u8* buf,u8 len)
{
	u8 t;
	
  for(t=0;t<len;t++)		        //循环发送数据
	{		   
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART5,buf[t]);
	}	 
 	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);		  	
}


/*****************************************************
 * 函数名：uart_init
 * 描述  ：串口初始化函数
 * 硬件  ：TX4--->PC10 RX4--->PC11
 *         TX5--->PC12 RX5--->PD2
 * 输入  ：波特率
 * 输出  ：无
 ****************************************************/
void uart_init(u32 bound1,u32 bound2,u32 bound3,u32 bound4,u32 bound5)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//uart1-3 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB1Periph_USART2, ENABLE);
	//uart4-5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4|RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
	//复位串口
 	USART_DeInit(USART1);  
  USART_DeInit(USART2);
	USART_DeInit(USART3);
	USART_DeInit(UART4);
	USART_DeInit(UART5);
	
	
  //串口1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
	//串口2	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//串口3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//串口4 TX4--->PC10 RX4--->PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//串口5 TX5--->PC12 RX5--->PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure); 




  //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound1;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口
	
	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound2;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口
	
	//USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound3;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口
	
	//UART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound4;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure); //初始化串口
 
 	//UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound5;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART5, &USART_InitStructure); //初始化串口
 
 
 
 
 //USART1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	
	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	
  //USART3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	  //UART4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	  //UART5 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断


  USART_Cmd(USART1, ENABLE);                    //使能串口 1
	USART_Cmd(USART2, ENABLE);                    //使能串口 2
  USART_Cmd(USART3, ENABLE);                    //使能串口 3
	 
	USART_Cmd(UART4, ENABLE);                    	//使能串口 4
  USART_Cmd(UART5, ENABLE);                    	//使能串口 5

}

#if USART_printf_EN 
/*****************************************************
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART_printf()调用
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
 * 函数名：USART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，USART1、USART3、USART3
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART_printf( USART3, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
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
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
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
