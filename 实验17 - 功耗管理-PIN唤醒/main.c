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
** Descriptions:		功耗管理-PIN唤醒实验				
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
#include "hal_rtc.h"

/*-------------------管脚定义--------------------------------------------------*/
#define  D1    P00  // 开发板上的指示灯D1
#define  D2    P01  // 开发板上的指示灯D2
#define  S1    P12  // 开发板上的按键S1
#define  S2    P13  // 开发板上的按键S2


/* 开发板中nRF24LE1管脚配置
P00：输出，驱动D1	       P12：输入，按键检测S1  
P01：输出，驱动D2			   P13：输入，按键检测S2
P02：输出，OLED时钟			 P14：输出，OLED数据
P03：输出，UART TXD			 P15：输出，OLED片选
P04：输入，UART RXD			 P16：输出，OLED命令/数据选择
P06：输入，AIN6  AD检测  蜂鸣器驱动(通过跳线选择)
*/
						 
#define  DEEPSLEEP        1  // 深度睡眠
#define  MEMRET_TIMEOFF   2	 // 存储器维持，定时器关闭
#define  MEMRET_TIMEON    3	 // 存储器维持，定时器开启
#define  REGRET           4	 // 寄存器维持


//函数声明
void GetPrintLastPWM(void);

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
 * 描  述 : 初始化时钟
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void mcu_init(void)
{
	hal_rtc_start(false); //关闭32.768KHz时钟
  hal_clklf_set_source(HAL_CLKLF_RCOSC32K); //32.768KHz的时钟源为内部RC	   
	hal_clk_set_16m_source(HAL_CLK_XOSC16M);  //使用外部16MHz晶振

	hal_clk_regret_xosc16m_on(false);         //寄存器维持模式下关闭 16MHz时钟
  hal_rtc_start(true);              
	
  while((CLKLFCTRL&0x80)==0x80);	         //等待时钟启动完成
  while((CLKLFCTRL&0x80)!=0x80);
}

/*******************************************************************************************************
 * 描  述 : 设置nRF24LE1低功耗模式
 * 入  参 : mode:低功耗模式
 * 返回值 : 无
 *******************************************************************************************************/
void SetPowrDownMode(uint8_t mode)
{
  uint8_t PowrMode;
  
  switch(mode)
  {
    case DEEPSLEEP  :     // 深度睡眠，唤醒后复位
	    PowrMode=0x01;
	    break;
    case MEMRET_TIMEOFF : // 存储器维持，定时器关闭，唤醒后复位
	    PowrMode=0x02;
	    break;
    case MEMRET_TIMEON:	  // 存储器维持，定时器开启，唤醒后复位
	    PowrMode=0x03;
	    break;
    case REGRET :         // 寄存器维持
	    PowrMode=0x04;
	    break;
    default         :	   //运行
	    PowrMode=0x00;
	    break;
  }

  if(PowrMode==0x01)hal_clk_set_16m_source(HAL_CLK_RCOSC16M);// 进入到DEEPSLEEP前一定要启用RC时钟源 
  if((PowrMode==0x01) || (PowrMode==0x02) || (PowrMode==0x03))OPMCON|=0x02;	 // 这3种低功耗模式唤醒后nRF24LE1会复位，所以要锁定IO     

  PWRDWN = PowrMode; 
  PWRDWN = 0x00;  // Clear power down
}

/*******************************************************************************************************
 * 描  述 : 获取上一次低功耗模式，并通过串口打印
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void GetPrintLastPWM(void)
{
  uint8_t PowrMode;

  PowrMode = PWRDWN & 0x07;

  switch(PowrMode)
  {
    case 0x00:
	  PutString("Last mode:Power Off!"); 
	  break;
    case 0x01:
	  PutString("Last mode:Deep Sleep!");
	  break;
    case 0x02:
	  PutString("Last mode:Memory Retention, Timer Off!");
	  break;
    case 0x03:
	  PutString("Last mode:Memory Retention, Timer On!");
	  break;
    case 0x04:
	  PutString("Last mode:Register Retention!");
	  break;
    default  :
	  PutString("Error Or Reserved!");
	  break;
  }
}
/*******************************************************************************************************
 * 描  述 : 设置唤醒PIN
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void SetWakeUpPin(void)
{
  OPMCON=0x04;               /* 开锁，低电平唤醒          */ 
  WUOPC0=0x00;               /* P0唤醒的引脚设置：无      */
  WUOPC1=0x04;               /* P1唤醒的引脚设置：P12     */
  
  P1DIR|=0x04;               /* P12初始化为输入I/O口      */
  P12   =1;                  /* P12初始化为高电平         */ 	             
}
/*******************************************************************************************************
 * 描  述 : 主函数
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t i,ReceDat;
  uint32_t LoopCount = 0;

  IO_Init();      //配置IO  
  SetWakeUpPin(); //设置唤醒管脚
  
  mcu_init();
  
  hal_uart_init(UART_BAUD_57K6); // 初始化UART0                     
  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //等待时钟稳定
  ; 
  
  EA = 1;             //使能全局中断
  for(i=0;i<10;i++)	  //指示灯闪烁5次，指示nEF24LE1启动
  {  
    D1 = ~D1;
    delay_ms(120);
  }  
  	      
  PutString("Program starting...\n");
  delay_ms(100);
  GetPrintLastPWM();
  delay_ms(100);
  
  while(1)
  {	
	  LoopCount++;
	  if(LoopCount == 10000)
	  {
	    D2 = ~D2;
	    LoopCount = 0;
	  }

    if( hal_uart_chars_available() )                  
    {
	    ReceDat = hal_uart_getchar();

	    switch(ReceDat)
	    {
	  	  case '1':	  // 深度睡眠，唤醒后复位
			    PutString("Enter powerdown mode:DEEPSLEEP\n");
	        delay_ms(20);
	        SetPowrDownMode(DEEPSLEEP);
			    break;

	      case '2':	 //存储器维持，定时器关闭，唤醒后复位
			    PutString("Enter powerdown mode:Memory retention, Timer Off\n");
	        delay_ms(20);
	        SetPowrDownMode(MEMRET_TIMEOFF);
			    break;

	      case '3':	 //存储器维持，定时器开启，唤醒后复位
			    PutString("Enter powerdown mode:Memory retention, Timer On\n");
	        delay_ms(20);
	        SetPowrDownMode(MEMRET_TIMEON);
			    break;

		   case '4':	//寄存器维持
			    PutString("Enter powerdown mode:Register retention\n");
	        delay_ms(20);
	        SetPowrDownMode(REGRET);			
			    break;

		   default:
			   PutString("ERR\n");
			   break;
	    }
    }	 
  }
}
 /*********************************END FILE****************************************************************/	