/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date: 2015-4-16         
** Last Version:		   1.2
** Descriptions:							
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2014-11-12
** Version:			    1.0
** Descriptions:		看门狗实验					
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
#include "hal_rtc.h"
#include "hal_delay.h"
#include "hal_wdog.h"

/*-------------------管脚定义--------------------------------------------------*/
#define  D1    P00  // 开发板上的指示灯D1


/* 开发板中nRF24LE1管脚配置
P00：输出，驱动D1	       P12：输入，按键检测S1  
P01：输出，驱动D2			   P13：输入，按键检测S2
P02：输出，OLED时钟			 P14：输出，OLED数据
P03：输出，UART TXD			 P15：输出，OLED片选
P04：输入，UART RXD			 P16：输出，OLED命令/数据选择
P06：输入，AIN6  AD检测  蜂鸣器驱动(通过跳线选择)
*/

#define  WDSV_10S    1280              //看门狗超时时间：10s 
/*
** 看门狗超时时间计算	
**  超时时间 = (WDSV * 256)/32768
**  所以：最小看门狗超时周期 = 7.8125ms
**        最大看门狗超时周期 = 512s	
*/


/*******************************************************************************************************
 * 描  述 : 配置 IO P0.0和P0.1为输出，驱动LED
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void IoInit(void)
{
  P0DIR &= ~0x01;    //配置P0.0为输出
	P0DIR &= ~0x08;    //P03：输出，UART TXD
	P0DIR |= 0x10;     //P04：输入，UART RXD
  D1 = 1;            //设置D1初始状态为熄灭
}

/*******************************************************************************************************
 * Description : 使能32.768KHz时钟，使用内部RC作为32.768KHz时钟源
 * Parameters  : none
 * Return      : none
 *******************************************************************************************************/
void ClockInit(void)
{
	hal_rtc_start(false);
  hal_clklf_set_source(HAL_CLKLF_RCOSC32K);           //32.768KHz的时钟源为内部RC  

  hal_rtc_start(true);              
	
  while((CLKLFCTRL&0x80)==0x80);	                    // Wait for the 32kHz to startup (change phase)
  while((CLKLFCTRL&0x80)!=0x80);   
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
  uint8_t RstReason;  //复位原因
  uint8_t i;
  
  IoInit();  //IO初始化

  hal_uart_init(UART_BAUD_57K6); //初始化UART，波特率57600                     

  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //等待时钟稳定
  ;

  
  EA = 1;        // 使能全局中断

  ClockInit();	 //初始化时钟	      

  for(i=0;i<6;i++)
  {
	  D1 = ~D1;
	  delay_ms(100);
  }

  RstReason = RSTREAS;	//获取前一次复位原因代码
  RstReason &= 0x07;

  switch(RstReason)
  {
     case 0x00:	//On-chip reset generator
	   PutString("Last reset reason: On-chip reset generatorOn-chip reset generator!\n");
	   break;

	 case 0x01:	//RST pin
	   PutString("Last reset reason: RST pin!\n");
	   break;

	 case 0x02:	//Watchdog
	   PutString("Last reset reason: Watchdog!\n");
	   break;

	 case 0x04:  //Reset from on-chip hardware debugger
	   PutString("Last reset reason: Reset from on-chip hardware debugger!\n");
	   break;

	 default:
	   break;
  }
  RSTREAS = 0; //RSTREAS的值是累加的，所以，读以后要进行清除
  hal_wdog_init(WDSV_10S);  //配置看门狗超时时间10s，使能看门狗

  while(1); //等待看门狗超时，复位系统

}
 /*********************************END FILE****************************************************************/	