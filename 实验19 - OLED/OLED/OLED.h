#ifndef OLED_H
#define OLED_H

#include <reg24le1.h>
#include <stdint.h>
#include "hal_delay.h"


#define OLED_WR_CMD     0
#define OLED_WR_DAT     1


#define SIZE 16

#define  LCD_SCL    P02  //OLED时钟
#define  LCD_SDA    P14  //OLED数据
#define  LCD_DC     P16  //OLED命令/数据选择 H=命令 L=数据
#define  LCD_CS     P15  //OLED片选

#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64



extern  void DelayMS(uint32_t dly);
extern  void OLED_Init(void);
extern  void OLED_Fill(uint8_t bmp_dat);
extern  void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
extern  void OLED_Set_Pos(uint8_t x, uint8_t y);
extern  void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[]);
extern  void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N);
extern  void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[]);
#endif
