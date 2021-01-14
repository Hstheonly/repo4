#ifndef __USART_H
#define __USART_H
#include <stdio.h>
#include "sys.h" 
#include <stdarg.h>


#define USART_printf_EN   1       //使能USART_printf()函数 (1)使能 (0)禁用  	
	
void uart_init(u32 bound1,u32 bound2,u32 bound3,u32 bound4,u32 bound5);
void USART1_Send_Data(u8* buf,u8 len);
void USART1_Receive_Data(u8 *buf,u8 *len);
void USART2_Send_Data(u8* buf,u8 len);
void USART2_Receive_Data(u8 *buf,u8 *len);
void USART3_Send_Data(u8* buf,u8 len);
void USART3_Receive_Data(u8 *buf,u8 *len);
void UART4_Send_Data(u8* buf,u8 len);
void UART4_Receive_Data(u8 *buf,u8 *len);
void UART5_Send_Data(u8* buf,u8 len);
void UART5_Receive_Data(u8 *buf,u8 *len);

#if USART_printf_EN 
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
#endif

#endif
