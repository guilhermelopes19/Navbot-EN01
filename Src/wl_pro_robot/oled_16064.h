#ifndef __oled_16064_h__
#define __oled_16064_h__

#include <Arduino.h>

#define SPI_SCK  0
#define SPI_MOSI 4
// #define SPI_MISO -1
#define SPI_CS   21
#define OLED_DC  15
#define OLED_RES -1

#define OLED_SCK_Clr() digitalWrite(SPI_SCK,LOW)
#define OLED_SCK_Set() digitalWrite(SPI_SCK,HIGH)

#define OLED_MOSI_Clr() digitalWrite(SPI_MOSI,LOW)
#define OLED_MOSI_Set() digitalWrite(SPI_MOSI,HIGH)

#define OLED_RES_Clr()  digitalWrite(OLED_RES,LOW)
#define OLED_RES_Set()  digitalWrite(OLED_RES,HIGH)

#define OLED_DC_Clr()   digitalWrite(OLED_DC,LOW)
#define OLED_DC_Set()   digitalWrite(OLED_DC,HIGH)

#define OLED_CS_Clr()   digitalWrite(SPI_CS,LOW)
#define OLED_CS_Set()   digitalWrite(SPI_CS,HIGH)

void OLED_WR_REG(uint8_t reg);
void OLED_WR_DATA8(uint8_t dat);


/*Define the parameters of liquid crystal resolution*/

#define OLED_W 160
#define OLED_H  64

/*Define the orientation of OLED display*/
/* 0:Positive display     */
/* 1;Rotate 180 degrees for display*/
#define USE_HORIZONTAL 0

void OLED_GPIOInit(void);//
void Column_Address(uint16_t a,uint16_t b); //Set column address
void Row_Address(uint16_t a,uint16_t b);//Set row address
void OLED_Fill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color);//
void OLED_ShowChar(uint16_t x,uint16_t y,uint8_t chr,uint16_t sizey,uint8_t mode);//Display a single character
void OLED_ShowString (uint16_t x,uint16_t y,char *dp,uint16_t sizey,uint8_t mode);//
void OLED_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t sizey,uint8_t mode);//
void OLED_DrawBMP(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t BMP[],uint8_t mode);//Display grayscale image
void OLED_DrawSingleBMP(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t BMP[],uint8_t mode);//Display a monochrome image
void OLED_Init(void);//
void OLED_Refresh(void);

#endif








