#include "oled.h"
#include "oledfont.h"  // 这里包含你自己生成的字库文件！
#include "main.h"

// 向 OLED 发送命令
void Write_IIC_Command(unsigned char IIC_Command) {
    IIC_Start();
    IIC_Send_Byte(0x78);            // 写入从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);            // 写入寄存器地址 (0x00 表示发送的是命令)
    IIC_Wait_Ack();
    IIC_Send_Byte(IIC_Command);     // 发送命令
    IIC_Wait_Ack();
    IIC_Stop();
}

// 向 OLED 发送数据
void Write_IIC_Data(unsigned char IIC_Data) {
    IIC_Start();
    IIC_Send_Byte(0x78);            // 写入从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);            // 写入寄存器地址 (0x40 表示发送的是显存数据)
    IIC_Wait_Ack();
    IIC_Send_Byte(IIC_Data);        // 发送数据
    IIC_Wait_Ack();
    IIC_Stop();
}

// 设置光标位置 (Page Addressing Mode)
// x: 0~127, y: 0~7 (因为屏幕被分为8个页，每页8像素高)
void OLED_Set_Pos(unsigned char x, unsigned char y) {
    Write_IIC_Command(0xb0 + y);
    Write_IIC_Command(((x & 0xf0) >> 4) | 0x10); // 设置列高地址
    Write_IIC_Command((x & 0x0f) | 0x01);        // 设置列低地址
}

// 清屏函数 (全涂黑)
void OLED_Clear(void) {
    uint8_t i, n;
    for(i = 0; i < 8; i++) {
        Write_IIC_Command(0xb0 + i);    // 设置页地址
        Write_IIC_Command(0x00);        // 设置列低地址
        Write_IIC_Command(0x10);        // 设置列高地址
        for(n = 0; n < 128; n++) {
            Write_IIC_Data(0);          // 清除所有数据
        }
    }
}

// 显示一个字符 (适配 8x16 字体)
// x: 0~127, y: 0~63 (这里兼容了将像素坐标转换为页坐标)
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size) {
    unsigned char c = 0, i = 0;
    c = chr - ' '; // 获取字符在字库数组中的偏移量

    if (x > 120) { x = 0; y = y + 2; } // 换行逻辑

// 显示 8x16 字符的上半部分 (占据1个Page)
OLED_Set_Pos(x, y / 8);
for(i = 0; i < 8; i++) {
    Write_IIC_Data(F8X16[c][i]);  // <--- 改成二维数组寻址 F8X16[c][i]
}
    
// 显示 8x16 字符的下半部分 (占据下一个Page)
OLED_Set_Pos(x, y / 8 + 1);
for(i = 0; i < 8; i++) {
    Write_IIC_Data(F8X16[c][i + 8]); // <--- 改成二维数组寻址 F8X16[c][i + 8]
}
}

// 显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size) {
    unsigned char j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8; // 每个字符宽8像素
        if (x > 120) {
            x = 0;
            y += 16;
        }
        j++;
    }
}

// OLED 初始化序列 (经典的 SSD1306 厂家推荐上电时序)
void OLED_Init(void) {
    IIC_Init();       // 初始化底层的引脚
    delay_ms(200);    // 等待屏幕上电稳定

    Write_IIC_Command(0xAE); // 关闭显示
    Write_IIC_Command(0x00); // 设置列低位地址
    Write_IIC_Command(0x10); // 设置列高位地址
    Write_IIC_Command(0x40); // 设置起始行地址
    Write_IIC_Command(0x81); // 设置对比度控制寄存器
    Write_IIC_Command(0xCF); // 对比度值
    Write_IIC_Command(0xA1); // 设置段重定向
    Write_IIC_Command(0xC8); // 设置COM扫描方向
    Write_IIC_Command(0xA6); // 正常显示 (非反色)
    Write_IIC_Command(0xA8); // 设置多路复用率
    Write_IIC_Command(0x3f); 
    Write_IIC_Command(0xD3); // 设置显示偏移
    Write_IIC_Command(0x00); 
    Write_IIC_Command(0xD5); // 设置显示时钟分频比/振荡器频率
    Write_IIC_Command(0x80); 
    Write_IIC_Command(0xD9); // 设置预充电周期
    Write_IIC_Command(0xF1); 
    Write_IIC_Command(0xDA); // 设置COM硬件引脚配置
    Write_IIC_Command(0x12); 
    Write_IIC_Command(0xDB); // 设置VCOMH取消选择级别
    Write_IIC_Command(0x40); 
    Write_IIC_Command(0x20); // 设置内存寻址模式
    Write_IIC_Command(0x02); // 页寻址模式 (Page Addressing Mode)
    Write_IIC_Command(0x8D); // 电荷泵设置
    Write_IIC_Command(0x14); // 开启电荷泵
    Write_IIC_Command(0xA4); // 全局显示开启
    Write_IIC_Command(0xA6); // 正常显示
    Write_IIC_Command(0xAF); // 开启显示
    
    OLED_Clear(); // 上电清个屏，防止出现雪花点
}
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, const uint8_t BMP[]) {
    uint16_t j = 0;
    uint8_t i, m;
    
    // 计算需要占据多少个 Page (每 8 个像素高为一个 Page)
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0); 
    
    for (i = 0; i < sizey; i++) {
        OLED_Set_Pos(x, i + y); // 设置起始页和列
        for (m = 0; m < sizex; m++) {
            Write_IIC_Data(BMP[j++]); // 连续写入图像数据
        }
    }
}