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
** Descriptions:		OLEDʵ��		
**--------------------------------------------------------------------------------------------------------
** Modified by:			FiYu
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
#include <reg24le1.h>
#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>
#include "hal_delay.h"
#include "OLED.h"
#include "OLEDFONT.h"



/* ��������nRF24LE1�ܽ�����
P00�����������D1	       P12�����룬�������S1  
P01�����������D2			   P13�����룬�������S2
P02�������OLEDʱ��			 P14�������OLED����
P03�������UART TXD			 P15�������OLEDƬѡ
P04�����룬UART RXD			 P16�������OLED����/����ѡ��
P06�����룬AIN6  AD���  ����������(ͨ������ѡ��)
*/

/*******************************************************************************************************
 * Description : main loop
 * Parameters  : none
 * Return      : none
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t i=0; 
	
  OLED_Init();                      //oled ��ʼ��  
  OLED_Fill(0x00);                  //���� 
  while(true)
  {
    //��ʾACSII ʹ�õ����ַ���ʾ����
		OLED_Fill(0x00);
    for(i=0; i<16 ;i++)
    {
      OLED_ShowChar(i*8,0,' '+i);
      OLED_ShowChar(i*8,2,' '+i+16);  
      OLED_ShowChar(i*8,4,' '+i+32);
      OLED_ShowChar(i*8,6,' '+i+48);    
    }
    delay_ms(3000);
        
    //��ʾ���ֺ���ĸ ʹ���ַ�����ʾ����
    OLED_Fill(0x00);
    LCD_P8x16Str(0,0,"0123456789");
    LCD_P8x16Str(0,2,"abcdefghijklmnop");
    LCD_P8x16Str(0,4,"ABCDEFGHIJKLMNOP");
    LCD_P8x16Str(0,6,"0123456789");
    delay_ms(5000); 
        
    //��ʾ����
    OLED_Fill(0x00);
    for(i=0; i<6; i++)
    {
      LCD_P16x16Ch(i*16+16,2,i);    
    }
    for(i=0; i<4; i++)
    {
      LCD_P16x16Ch(i*16+32,4,i+6); 
      LCD_P16x16Ch(i*16+32,6,i+6+4);
    }
    delay_ms(3000);      
		
		//��ʾͼ��
    OLED_Fill(0x00);
    OLED_DrawBMP(0,0,128,8,BMP1);
    delay_ms(3000); 
        
    OLED_Fill(0x00);
    OLED_DrawBMP(0,0,128,8,BMP2);
    delay_ms(3000); 
            
  }
}
 /*********************************END FILE****************************************************************/	