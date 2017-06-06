#include <reg24le1.h>
#include "OLED.h"
#include "OLEDFONT.h"


/******************************************************************************
 * 描  述 : 向OLED写入1字节数据
 * 入  参 : dat:数据；mode：=0：写入命令，=1：写入数据
 * 返回值 : 无
 ******************************************************************************/
void OLED_WrByte(uint8_t dat,uint8_t mode)     
{
    uint8_t i=8, temp=0;
    
    if(mode == OLED_WR_CMD)LCD_DC = 0;
    else LCD_DC = 1;
      
    for(i=0; i<8; i++) //发送一个八位数据 
    {
        LCD_SCL = 0;  
        
        temp = dat&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        dat <<= 1;
				LCD_SCL = 1;             
            
    }
}
 
/******************************************************************************
 * 描  述 : 设置坐标
 * 入  参 : x：x坐标；y：y坐标
 * 返回值 : 无
 ******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    OLED_WrByte((0xb0+y),OLED_WR_CMD);
    OLED_WrByte(((x&0xf0)>>4)|0x10,OLED_WR_CMD);
    OLED_WrByte((x&0x0f)|0x01,OLED_WR_CMD); 
} 
/******************************************************************************
 * 描  述 : LCD初始化
 * 入  参 : 无
 * 返回值 : 无
 ******************************************************************************/
void OLED_Fill(uint8_t dat) 
{
    uint8_t y,x;
    for(y=0;y<8;y++)
    {
        OLED_WrByte(0xb0+y,OLED_WR_CMD);//设置页地址（0~7）
        OLED_WrByte(0x02,OLED_WR_CMD); //设置显示位置—列低地址
        OLED_WrByte(0x10,OLED_WR_CMD); //设置显示位置—列高地址
        for(x=0; x<X_WIDTH; x++)
            OLED_WrByte(dat,OLED_WR_DAT);
    }
}

/******************************************************************************
 * 描  述 : 指定位置显示一个字符
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  uint8_t c=0,i=0;	
  
  c = chr-' ';//得到偏移后的值			
  if(x > Max_Column-1)
  {
    x = 0;
    y = y+2;
  }

  OLED_Set_Pos(x,y);	
  for(i=0; i<8; i++)OLED_WrByte(F8X16[c*16+i],OLED_WR_DAT); 
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)OLED_WrByte(F8X16[c*16+i+8],OLED_WR_DAT);
}

/******************************************************************************
 * 描  述 : 显示8*16一组标准ASCII字符串
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[])
{
  uint8_t c=0,i=0,j=0;
    
  while (ch[j] != '\0')
  {    
    c = ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);    
    for(i=0; i<8; i++)OLED_WrByte(F8X16[c*16+i],OLED_WR_DAT);          
    OLED_Set_Pos(x,y+1);    
    for(i=0;i<8;i++) OLED_WrByte(F8X16[c*16+i+8],OLED_WR_DAT);        
    x += 8;
    j++;
  }
}
/******************************************************************************
 * 描  述 : 显示汉字
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm=0;
    unsigned int adder=32*N;        
    OLED_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)               
    {
        OLED_WrByte(F16x16[adder],OLED_WR_DAT);    
        adder += 1;
    }      
    OLED_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++)         
    {
        OLED_WrByte(F16x16[adder],OLED_WR_DAT);
        adder += 1;
    }           
}
/******************************************************************************
 * 描  述 : 显示BMP图片128×64
 * 入  参 : 起始点坐标(x,y),x的范围0～127，y为页的范围0～7
 * 返回值 : 无
 ******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[])
{ 	
  uint16_t j=0;
  uint8_t x,y;
  
  if((y1 % 8) == 0) y = y1/8;      
  else y = y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0; x<x1; x++)
    {      
      OLED_WrByte(BMP[j++],OLED_WR_DAT);	    	
    }
  }
} 

/******************************************************************************
 * 描  述 : LCD初始化
 * 入  参 : 无
 * 返回值 : 无
 ******************************************************************************/
void OLED_Init(void)     
{  
    P0DIR &= ~0x04;	   //配置P0.2为输出
	  P1DIR &= ~0x70;	   //配置P1.4 P1.5 P1.6为输出
    delay_ms(10);
	  LCD_CS=1;
	  delay_ms(10);
    LCD_CS=0;
    
    delay_ms(200);

    OLED_WrByte(0xae,OLED_WR_CMD);//--turn off oled panel
    OLED_WrByte(0x00,OLED_WR_CMD);//---set low column address
    OLED_WrByte(0x10,OLED_WR_CMD);//---set high column address
    OLED_WrByte(0x40,OLED_WR_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WrByte(0x81,OLED_WR_CMD);//--set contrast control register
    OLED_WrByte(0xcf,OLED_WR_CMD); // Set SEG Output Current Brightness
	
    OLED_WrByte(0xa1,OLED_WR_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WrByte(0xc8,OLED_WR_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WrByte(0xa6,OLED_WR_CMD);//--set normal display
    OLED_WrByte(0xa8,OLED_WR_CMD);//--set multiplex ratio(1 to 64)
    OLED_WrByte(0x3f,OLED_WR_CMD);//--1/64 duty
	
    OLED_WrByte(0xd3,OLED_WR_CMD);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WrByte(0x00,OLED_WR_CMD);//-not offset
		
    OLED_WrByte(0xd5,OLED_WR_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WrByte(0x80,OLED_WR_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WrByte(0xd9,OLED_WR_CMD);//--set pre-charge period
		
    OLED_WrByte(0xf1,OLED_WR_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		//OLED_WrByte(0x22,OLED_WR_CMD);
    OLED_WrByte(0xda,OLED_WR_CMD);//--set com pins hardware configuration
    OLED_WrByte(0x12,OLED_WR_CMD);
    OLED_WrByte(0xdb,OLED_WR_CMD);//--set vcomh
		
    OLED_WrByte(0x40,OLED_WR_CMD);//Set VCOM Deselect Level
		
    OLED_WrByte(0x20,OLED_WR_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WrByte(0x02,OLED_WR_CMD);//
    OLED_WrByte(0x8d,OLED_WR_CMD);//--set Charge Pump enable/disable
    OLED_WrByte(0x14,OLED_WR_CMD);//--set(0x10) disable
    OLED_WrByte(0xa4,OLED_WR_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WrByte(0xa6,OLED_WR_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WrByte(0xaf,OLED_WR_CMD);//--turn on oled panel
    OLED_Fill(0xff);  //初始清屏
    OLED_Set_Pos(0,0);     
}
/*********************************END FILE*************************************/




