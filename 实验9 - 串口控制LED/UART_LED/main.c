/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date: 2015-4-16         
** Last Version:		1.2
** Descriptions:							
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2014-11-12
** Version:			    1.0
** Descriptions:		UART控制LED实验					
**--------------------------------------------------------------------------------------------------------
** Modified by:			FiYu
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
#include <stdio.h> 
#include <reg24le1.h>
#include <stdint.h>
#include "hal_uart.h"
#include "hal_clk.h"
#include "hal_delay.h"


/*-------------------管脚定义--------------------------------------------------*/
#define  D1    P00  // 开发板上的指示灯D1
#define  D2    P01  // 开发板上的指示灯D2

#define  LED_ON     0 //点亮指示灯
#define  LED_OFF    1 //熄灭指示灯
#define  RXBUF_LEN  3 //UART接收缓存字节数


/* 开发板中nRF24LE1管脚配置
P00：输出，驱动D1	       P12：输入，按键检测S1  
P01：输出，驱动D2			   P13：输入，按键检测S2
P02：输出，OLED时钟			 P14：输出，OLED数据
P03：输出，UART TXD			 P15：输出，OLED片选
P04：输入，UART RXD			 P16：输出，OLED命令/数据选择
P06：输入，AIN6  AD检测  蜂鸣器驱动(通过跳线选择)
*/

/*******************************************************************************************************
 * 描  述 : 读取UART接收的数据
 * 入  参 : 无
 * 返回值 : 读取的数据
 *******************************************************************************************************/
char getchar(void)
{
  return hal_uart_getchar();
}

/*******************************************************************************************************
 * 描  述 : 配置 IO P0.0和P0.1为输出P03：输出,作为UART TXD，P04：输入,作为UART RXD
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void IO_Init(void)
{	
	P0DIR &= ~0x03;	   //配置P0.0和P0.1为输出
	P0DIR &= ~0x08;    //P03：输出，UART TXD
	P0DIR |= 0x10;     //P04：输入，UART RXD	
	D1 = 1;            //设置D1初始状态为熄灭
	D2 = 1;	           //设置D2初始状态为熄灭
}
/*******************************************************************************************************
 * 描  述 : 串口输出字符串
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void PutString(char *s)
{
  while(*s != 0)
    hal_uart_putchar(*s++);
}
/*******************************************************************************************************
 * 描  述 : 主函数
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t UartRxBuf[RXBUF_LEN];  //UART接收缓存
	uint8_t RxCnt = 0;             //UART接收字节数
	uint8_t RxByte;
  
  IO_Init();  //配置IO  
  hal_uart_init(UART_BAUD_57K6); // 初始化UART0                     

  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M)//等待时钟稳定 
  ;
 
  EA = 1;  // 使能全局中断                                           

  PutString("Welcome to FiYu!\n");

  while(1)
  {

    while(hal_uart_chars_available())
		{
			RxByte = hal_uart_getchar();
			if((RxByte != '#')&&(RxCnt < 3))
			{
				UartRxBuf[RxCnt++] = RxByte;
			}
			else
			{
				if(RxCnt >= 3)
				{
					RxCnt = 0;
				}
				else
				{
					if((UartRxBuf[0] == 'D') || (UartRxBuf[0] == 'd'))
					{
						switch(UartRxBuf[1]-48)
						{
							case 1:
								D1 = 0;
							  D2 = 1;
							  RxCnt = 0;
							  break;
							
							case 2:
								D1 = 1;
							  D2 = 0;
							  RxCnt = 0;
							  break;
							
							default:
								break;
						}
					}
				}
			}	
		}
  }
}
/*********************************END FILE****************************************************************/