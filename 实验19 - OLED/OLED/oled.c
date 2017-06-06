#include <reg24le1.h>
#include "OLED.h"
#include "OLEDFONT.h"


/******************************************************************************
 * ��  �� : ��OLEDд��1�ֽ�����
 * ��  �� : dat:���ݣ�mode��=0��д�����=1��д������
 * ����ֵ : ��
 ******************************************************************************/
void OLED_WrByte(uint8_t dat,uint8_t mode)     
{
    uint8_t i=8, temp=0;
    
    if(mode == OLED_WR_CMD)LCD_DC = 0;
    else LCD_DC = 1;
      
    for(i=0; i<8; i++) //����һ����λ���� 
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
 * ��  �� : ��������
 * ��  �� : x��x���ꣻy��y����
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    OLED_WrByte((0xb0+y),OLED_WR_CMD);
    OLED_WrByte(((x&0xf0)>>4)|0x10,OLED_WR_CMD);
    OLED_WrByte((x&0x0f)|0x01,OLED_WR_CMD); 
} 
/******************************************************************************
 * ��  �� : LCD��ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Fill(uint8_t dat) 
{
    uint8_t y,x;
    for(y=0;y<8;y++)
    {
        OLED_WrByte(0xb0+y,OLED_WR_CMD);//����ҳ��ַ��0~7��
        OLED_WrByte(0x02,OLED_WR_CMD); //������ʾλ�á��е͵�ַ
        OLED_WrByte(0x10,OLED_WR_CMD); //������ʾλ�á��иߵ�ַ
        for(x=0; x<X_WIDTH; x++)
            OLED_WrByte(dat,OLED_WR_DAT);
    }
}

/******************************************************************************
 * ��  �� : ָ��λ����ʾһ���ַ�
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  uint8_t c=0,i=0;	
  
  c = chr-' ';//�õ�ƫ�ƺ��ֵ			
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
 * ��  �� : ��ʾ8*16һ���׼ASCII�ַ���
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
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
 * ��  �� : ��ʾ����
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
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
 * ��  �� : ��ʾBMPͼƬ128��64
 * ��  �� : ��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
 * ����ֵ : ��
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
 * ��  �� : LCD��ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Init(void)     
{  
    P0DIR &= ~0x04;	   //����P0.2Ϊ���
	  P1DIR &= ~0x70;	   //����P1.4 P1.5 P1.6Ϊ���
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
	
    OLED_WrByte(0xa1,OLED_WR_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    OLED_WrByte(0xc8,OLED_WR_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
    OLED_Fill(0xff);  //��ʼ����
    OLED_Set_Pos(0,0);     
}
/*********************************END FILE*************************************/




