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
** Descriptions:		���Ĺ���-PIN����ʵ��				
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

/*-------------------�ܽŶ���--------------------------------------------------*/
#define  D1    P00  // �������ϵ�ָʾ��D1
#define  D2    P01  // �������ϵ�ָʾ��D2
#define  S1    P12  // �������ϵİ���S1
#define  S2    P13  // �������ϵİ���S2


/* ��������nRF24LE1�ܽ�����
P00�����������D1	       P12�����룬�������S1  
P01�����������D2			   P13�����룬�������S2
P02�������OLEDʱ��			 P14�������OLED����
P03�������UART TXD			 P15�������OLEDƬѡ
P04�����룬UART RXD			 P16�������OLED����/����ѡ��
P06�����룬AIN6  AD���  ����������(ͨ������ѡ��)
*/
						 
#define  DEEPSLEEP        1  // ���˯��
#define  MEMRET_TIMEOFF   2	 // �洢��ά�֣���ʱ���ر�
#define  MEMRET_TIMEON    3	 // �洢��ά�֣���ʱ������
#define  REGRET           4	 // �Ĵ���ά��


//��������
void GetPrintLastPWM(void);

/*******************************************************************************************************
 * ��  �� : ���� IO P0.0��P0.1Ϊ����PP03�����,��ΪUART TXD��P04������,��ΪUART RXD
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void IO_Init(void)
{	
	P0DIR &= ~0x03;	   //����P0.0��P0.1Ϊ���
	P0DIR &= ~0x08;    //P03�������UART TXD
	P0DIR |= 0x10;     //P04�����룬UART RXD	
	D1 = 1;            //����D1��ʼ״̬ΪϨ��
	D2 = 1;	           //����D2��ʼ״̬ΪϨ��
}

/*******************************************************************************************************
 * ��  �� : ���ڴ�ӡ16�����ַ�
 * ��  �� : ������ַ�
 * ����ֵ : ��
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
 * ��  �� : ��������ַ���
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void PutString(char *s)
{
  while(*s != 0)
    hal_uart_putchar(*s++);
}
/*******************************************************************************************************
 * ��  �� : ��ʼ��ʱ��
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void mcu_init(void)
{
	hal_rtc_start(false); //�ر�32.768KHzʱ��
  hal_clklf_set_source(HAL_CLKLF_RCOSC32K); //32.768KHz��ʱ��ԴΪ�ڲ�RC	   
	hal_clk_set_16m_source(HAL_CLK_XOSC16M);  //ʹ���ⲿ16MHz����

	hal_clk_regret_xosc16m_on(false);         //�Ĵ���ά��ģʽ�¹ر� 16MHzʱ��
  hal_rtc_start(true);              
	
  while((CLKLFCTRL&0x80)==0x80);	         //�ȴ�ʱ���������
  while((CLKLFCTRL&0x80)!=0x80);
}

/*******************************************************************************************************
 * ��  �� : ����nRF24LE1�͹���ģʽ
 * ��  �� : mode:�͹���ģʽ
 * ����ֵ : ��
 *******************************************************************************************************/
void SetPowrDownMode(uint8_t mode)
{
  uint8_t PowrMode;
  
  switch(mode)
  {
    case DEEPSLEEP  :     // ���˯�ߣ����Ѻ�λ
	    PowrMode=0x01;
	    break;
    case MEMRET_TIMEOFF : // �洢��ά�֣���ʱ���رգ����Ѻ�λ
	    PowrMode=0x02;
	    break;
    case MEMRET_TIMEON:	  // �洢��ά�֣���ʱ�����������Ѻ�λ
	    PowrMode=0x03;
	    break;
    case REGRET :         // �Ĵ���ά��
	    PowrMode=0x04;
	    break;
    default         :	   //����
	    PowrMode=0x00;
	    break;
  }

  if(PowrMode==0x01)hal_clk_set_16m_source(HAL_CLK_RCOSC16M);// ���뵽DEEPSLEEPǰһ��Ҫ����RCʱ��Դ 
  if((PowrMode==0x01) || (PowrMode==0x02) || (PowrMode==0x03))OPMCON|=0x02;	 // ��3�ֵ͹���ģʽ���Ѻ�nRF24LE1�Ḵλ������Ҫ����IO     

  PWRDWN = PowrMode; 
  PWRDWN = 0x00;  // Clear power down
}

/*******************************************************************************************************
 * ��  �� : ��ȡ��һ�ε͹���ģʽ����ͨ�����ڴ�ӡ
 * ��  �� : ��
 * ����ֵ : ��
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
 * ��  �� : ���û���PIN
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void SetWakeUpPin(void)
{
  OPMCON=0x04;               /* �������͵�ƽ����          */ 
  WUOPC0=0x00;               /* P0���ѵ��������ã���      */
  WUOPC1=0x04;               /* P1���ѵ��������ã�P12     */
  
  P1DIR|=0x04;               /* P12��ʼ��Ϊ����I/O��      */
  P12   =1;                  /* P12��ʼ��Ϊ�ߵ�ƽ         */ 	             
}
/*******************************************************************************************************
 * ��  �� : ������
 * ��  �� : ��
 * ����ֵ : ��
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t i,ReceDat;
  uint32_t LoopCount = 0;

  IO_Init();      //����IO  
  SetWakeUpPin(); //���û��ѹܽ�
  
  mcu_init();
  
  hal_uart_init(UART_BAUD_57K6); // ��ʼ��UART0                     
  while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M) //�ȴ�ʱ���ȶ�
  ; 
  
  EA = 1;             //ʹ��ȫ���ж�
  for(i=0;i<10;i++)	  //ָʾ����˸5�Σ�ָʾnEF24LE1����
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
	  	  case '1':	  // ���˯�ߣ����Ѻ�λ
			    PutString("Enter powerdown mode:DEEPSLEEP\n");
	        delay_ms(20);
	        SetPowrDownMode(DEEPSLEEP);
			    break;

	      case '2':	 //�洢��ά�֣���ʱ���رգ����Ѻ�λ
			    PutString("Enter powerdown mode:Memory retention, Timer Off\n");
	        delay_ms(20);
	        SetPowrDownMode(MEMRET_TIMEOFF);
			    break;

	      case '3':	 //�洢��ά�֣���ʱ�����������Ѻ�λ
			    PutString("Enter powerdown mode:Memory retention, Timer On\n");
	        delay_ms(20);
	        SetPowrDownMode(MEMRET_TIMEON);
			    break;

		   case '4':	//�Ĵ���ά��
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