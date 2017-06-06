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
** Descriptions:		OLED实验		
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



/* 开发板中nRF24LE1管脚配置
P00：输出，驱动D1	       P12：输入，按键检测S1  
P01：输出，驱动D2			   P13：输入，按键检测S2
P02：输出，OLED时钟			 P14：输出，OLED数据
P03：输出，UART TXD			 P15：输出，OLED片选
P04：输入，UART RXD			 P16：输出，OLED命令/数据选择
P06：输入，AIN6  AD检测  蜂鸣器驱动(通过跳线选择)
*/

/*******************************************************************************************************
 * Description : main loop
 * Parameters  : none
 * Return      : none
 *******************************************************************************************************/
void main(void)
{ 
  uint8_t i=0; 
	
  OLED_Init();                      //oled 初始化  
  OLED_Fill(0x00);                  //清屏 
  while(true)
  {
    //显示ACSII 使用单个字符显示函数
		OLED_Fill(0x00);
    for(i=0; i<16 ;i++)
    {
      OLED_ShowChar(i*8,0,' '+i);
      OLED_ShowChar(i*8,2,' '+i+16);  
      OLED_ShowChar(i*8,4,' '+i+32);
      OLED_ShowChar(i*8,6,' '+i+48);    
    }
    delay_ms(3000);
        
    //显示数字和字母 使用字符串显示函数
    OLED_Fill(0x00);
    LCD_P8x16Str(0,0,"0123456789");
    LCD_P8x16Str(0,2,"abcdefghijklmnop");
    LCD_P8x16Str(0,4,"ABCDEFGHIJKLMNOP");
    LCD_P8x16Str(0,6,"0123456789");
    delay_ms(5000); 
        
    //显示汉字
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
		
		//显示图形
    OLED_Fill(0x00);
    OLED_DrawBMP(0,0,128,8,BMP1);
    delay_ms(3000); 
        
    OLED_Fill(0x00);
    OLED_DrawBMP(0,0,128,8,BMP2);
    delay_ms(3000); 
            
  }
}
 /*********************************END FILE****************************************************************/	