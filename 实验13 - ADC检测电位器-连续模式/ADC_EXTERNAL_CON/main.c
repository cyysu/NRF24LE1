/****************************************Copyright (c)****************************************************
**                                        
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date: 2015-4-16         
** Last Version:		1.2
** Descriptions: ADC外部通道采样实验：连续模式
** ADC配置：
**   基准电压：VDD
**   通道：AIN6
**   分辨率：12位
**   采样模式：单次采样
**   采样速度：2ksps
**   数据对齐方式：右对齐
** 串口波特率：57600	
** 电压计算公式：V =（3.3）*采样值/4096							
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2014-11-12
** Version:			    1.0
** Descriptions:						
**--------------------------------------------------------------------------------------------------------
** Modified by:			FiYu
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
#include <reg24le1.h>
#include <stdint.h>
#include "hal_adc.h"
#include "hal_uart.h"
#include "hal_clk.h"
#include "hal_delay.h"


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


uint8_t SampingFined = 0;
uint8_t SampValueMsb = 0,SampValueLsb = 0;


//ADC初始化
void AdcInit(void)
{
  hal_adc_set_input_channel(HAL_INP_AIN6);            // 设置通道为AIN6                  
  hal_adc_set_reference(HAL_ADC_REF_VDD);             // 设置参考电压为VDD                  
  hal_adc_set_input_mode(HAL_ADC_SINGLE);             // 单端输入                
  hal_adc_set_conversion_mode(HAL_ADC_CONTINOUS);     // 连续模式 
	hal_adc_set_sampling_rate(HAL_ADC_2KSPS);           // 采样速度为2ksps
  hal_adc_set_resolution(HAL_ADC_RES_12BIT);          // 12位ADC                     
  hal_adc_set_data_just(HAL_ADC_JUST_RIGHT); 			    // 数据右对齐
	
	ADCDATH&=0XF0;	
	ADCDATL&=0X00;  
}

/*******************************************************************************************************
 * 描  述 : 配置 IO P0.0和P0.1为输出，驱动LED
 * 入  参 : 无
 * 返回值 : 无
 *******************************************************************************************************/
void IoInit(void)
{
  P0DIR &= ~0x01;    //配置P0.0 P0.1为输出
	P0DIR &= ~0x08;    //P03：输出，UART TXD
	P0DIR |= 0x10;     //P04：输入，UART RXD
  D1 = 1;            //设置D1初始状态为熄灭
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
/*********************************************************************************************************
** 描  述:  main函数															
** 入  参:  NONE		
** 返回值:  NONE																														
*********************************************************************************************************/
void main()
{
  IoInit();  //IO初始化

  hal_uart_init(UART_BAUD_57K6); //初始化UART，波特率57600                     

  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //等待时钟稳定
  ;
                                       
  AdcInit(); //ADC初始化
  MISC = 1; //使能ADC中断
	EA = 1;    //开启全局中断                                    
  hal_adc_start(); //启动ADC，进行连续采样
 
  while(1)
  {
	
	  delay_ms(500);	//延时500ms，避免串口输出过快，不方便观察转换结果
	  D1 = ~D1;		    //D4闪烁指示开发板工作正常
		
		if(SampingFined == 1)
	  {
	    SampingFined = 0;
			PutString("ADCC Samping Value(HEX):");		  
      PutHexString(SampValueMsb);  // 串口输出ADC转换值
      PutHexString(SampValueLsb);
	    PutString("\n");
	  }
  }  
}

/*********************************************************************************************************
** 描  述:  ADC中断服务函数		中断函数中置位采样标志并保存采样值												
** 入  参:  无		
** 返回值:  无																														
*********************************************************************************************************/
void adc_irq() interrupt INTERRUPT_MISCIRQ
{
  SampingFined = 1;
  SampValueMsb = hal_adc_read_MSB();
  SampValueLsb = hal_adc_read_LSB();
} 
/*****************************************End File***********************************************************/

