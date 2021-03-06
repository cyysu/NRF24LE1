/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date: 2015-4-16         
** Last Version:		   1.2
** Descriptions:		
**						
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2014-11-12
** Version:			    1.0
** Descriptions:		rng(Random number generator随机数发生器)实验程序
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
#include "hal_rng.h"


/*-------------------管脚定义--------------------------------------------------*/
#define  D1    P00  // 开发板上的指示灯D1
#define  D2    P01  // 开发板上的指示灯D2

/* 开发板中nRF24LE1管脚配置
P00：输出，驱动D1	       P12：输入，按键检测S1  
P01：输出，驱动D2			   P13：输入，按键检测S2
P02：输出，OLED时钟			 P14：输出，OLED数据
P03：输出，UART TXD			 P15：输出，OLED片选
P04：输入，UART RXD			 P16：输出，OLED命令/数据选择
P06：输入，AIN6  AD检测  蜂鸣器驱动(通过跳线选择)
*/

/*******************************************************************************************************
 * 描  述 : 配置 IO P0.0和P0.1为输出，驱动LED
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void IoInit(void)
{
  P0DIR &= ~0x03;    //配置P0.0 P0.1为输出
	P0DIR &= ~0x08;    //P03：输出，UART TXD
	P0DIR |= 0x10;     //P04：输入，UART RXD
  D1 = 1;            //设置D4初始状态为熄灭
	D2 = 1;	           //设置D5初始状态为熄灭	
}

/*******************************************************************************************************
 * 描  述 : 串口打印16进制字符
 * 入  参 : 输出的字符
 * 返回值 : 无
 *******************************************************************************************************/
void PutHexString (uint8_t x)
{
  uint8_t c;

  c = (x>>4)& 0xf;
  if (c > 9)
    hal_uart_putchar('A'+c-10);
  else
    hal_uart_putchar('0'+c);

  c = x & 0xf;
  if (c > 9)
    hal_uart_putchar('A'+c-10);
  else
    hal_uart_putchar('0'+c);
}
/*******************************************************************************************************
 * 描  述 : 串口打印字符串
 * 入  参 : 字符串地址
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
  uint8_t RngDat;
  
  IoInit();  //IO初始化

  hal_uart_init(UART_BAUD_57K6); //初始化UART，波特率57600                     
  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //等待时钟稳定
  ;
 
  EA = 1;// 使能全局中断
  hal_rng_bias_corr_enable(true); // 使能校正算法
  hal_rng_power_up(true);         // RNG上电		      

  while(1)
  {
	  delay_ms(500);	//延时500ms，避免串口输出过快，不方便观察转换结果
	  D1 = ~D1;		    //D4闪烁指示开发板工作正常
		
		if(hal_rng_data_ready() )
	  {
		  RngDat = hal_rng_read();
		  PutString("RNG Value(HEX):");		  
		  PutHexString(RngDat); 
      PutString("\n");			
	  }
  }
}
 /*********************************END FILE****************************************************************/	