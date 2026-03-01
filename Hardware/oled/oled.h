#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "my_iic.h"

// OLED IIC 器件地址 (一般默认是 0x78)
#define OLED_CMD  0    // 写命令
#define OLED_DATA 1    // 写数据

// ================= 函数声明 =================
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size);

// 底层通信函数
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, const uint8_t BMP[]);\

#endif