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
** Descriptions:		���Ź�ʵ��					
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

/*-------------------�ܽŶ���--------------------------------------------------*/
#define  D1    P00  // �������ϵ�ָʾ��D1


/* ��������nRF24LE1�ܽ�����
P00�����������D1	       P12�����룬�������S1  
P01�����������D2			   P13�����룬�������S2
P02�������OLEDʱ��			 P14�������OLED����
P03�������UART TXD			 P15�������OLEDƬѡ
P04�����룬UART RXD			 P16�������OLED����/����ѡ��
P06�����룬AIN6  AD���  ����������(ͨ������ѡ��)
*/

#define  WDSV_10S    1280              //���Ź���ʱʱ�䣺10s 
/*
** ���Ź���ʱʱ�����	
**  ��ʱʱ�� = (WDSV * 256)/32768
**  ���ԣ���С���Ź���ʱ���� = 7.8125ms
**        ����Ź���ʱ���� = 512s	
*/


/*******************************************************************************************************
 * ��  �� : ���� IO P0.0��P0.1Ϊ���������LED
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void IoInit(void)
{
  P0DIR &= ~0x01;    //����P0.0Ϊ���
	P0DIR &= ~0x08;    //P03�������UART TXD
	P0DIR |= 0x10;     //P04�����룬UART RXD
  D1 = 1;            //����D1��ʼ״̬ΪϨ��
}

/*******************************************************************************************************
 * Description : ʹ��32.768KHzʱ�ӣ�ʹ���ڲ�RC��Ϊ32.768KHzʱ��Դ
 * Parameters  : none
 * Return      : none
 *******************************************************************************************************/
void ClockInit(void)
{
	hal_rtc_start(false);
  hal_clklf_set_source(HAL_CLKLF_RCOSC32K);           //32.768KHz��ʱ��ԴΪ�ڲ�RC  

  hal_rtc_start(true);              
	
  while((CLKLFCTRL&0x80)==0x80);	                    // Wait for the 32kHz to startup (change phase)
  while((CLKLFCTRL&0x80)!=0x80);   
}

/*******************************************************************************************************
 * ��  �� : ���ڴ�ӡ�ַ���
 * ��  �� : �ַ�����ַ
 * ����ֵ : ��
 *******************************************************************************************************/
void PutString(char *s)
{
  while(*s != 0)
    hal_uart_putchar(*s++);
}

/*******************************************************************************************************
 * ��  �� : ������
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t RstReason;  //��λԭ��
  uint8_t i;
  
  IoInit();  //IO��ʼ��

  hal_uart_init(UART_BAUD_57K6); //��ʼ��UART��������57600                     

  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //�ȴ�ʱ���ȶ�
  ;

  
  EA = 1;        // ʹ��ȫ���ж�

  ClockInit();	 //��ʼ��ʱ��	      

  for(i=0;i<6;i++)
  {
	  D1 = ~D1;
	  delay_ms(100);
  }

  RstReason = RSTREAS;	//��ȡǰһ�θ�λԭ�����
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
  RSTREAS = 0; //RSTREAS��ֵ���ۼӵģ����ԣ����Ժ�Ҫ�������
  hal_wdog_init(WDSV_10S);  //���ÿ��Ź���ʱʱ��10s��ʹ�ܿ��Ź�

  while(1); //�ȴ����Ź���ʱ����λϵͳ

}
 /*********************************END FILE****************************************************************/	