#include "oled.h"
#include "oledfont.h"  
#include "main.h"

// =======================================================
// 1024 字节的全局显存 (128列 * 8页)
// =======================================================
uint8_t OLED_GRAM[8][128];

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

// 向 OLED 发送数据 (保留这个接口以防万一，显存刷新有专属的批量函数)
void Write_IIC_Data(unsigned char IIC_Data) {
    IIC_Start();
    IIC_Send_Byte(0x78);            
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);            // 0x40 表示发送的是显存数据
    IIC_Wait_Ack();
    IIC_Send_Byte(IIC_Data);        
    IIC_Wait_Ack();
    IIC_Stop();
}

// =======================================================
// 将显存数据一口气推送到屏幕 (突发连续写入 Burst Write)
// =======================================================
void OLED_Refresh_GRAM(void) {
    uint8_t i, n;		    
    for(i = 0; i < 8; i++) {  
        // 这里复用原本的写命令接口来定页地址
        Write_IIC_Command(0xb0 + i); 
        Write_IIC_Command(0x00);     
        Write_IIC_Command(0x10);        
        
        // 疯狂灌入 128 列的数据 (调用底层的 IIC_Send_Byte)
        IIC_Start();
        IIC_Send_Byte(0x78);         // 屏幕 IIC 地址
        IIC_Wait_Ack();	
        IIC_Send_Byte(0x40);         // 0x40 代表接下来全是数据 (Data)
        IIC_Wait_Ack();	
        
        for(n = 0; n < 128; n++) {
            IIC_Send_Byte(OLED_GRAM[i][n]);
            IIC_Wait_Ack();
        }
        IIC_Stop();
    }   
}

// 清空显存
void OLED_Clear(void) {  
    uint8_t i, n;		    
    for(i = 0; i < 8; i++) {
        for(n = 0; n < 128; n++) {
            OLED_GRAM[i][n] = 0x00;  
        }
    }
}

// 在显存中画一个字符
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size) {      	
    unsigned char c = 0, i = 0;	
    c = chr - ' '; 
    
    if(x > 120) { x = 0; y = y + 16; } 
    
    // 【新增】：安全锁！防止 Y 轴坐标超过 63 导致写爆 OLED_GRAM 数组
    if(y >= 64) return; 
    
    if(Char_Size == 16) {
        for(i = 0; i < 8; i++) OLED_GRAM[y/8][x+i] = F8X16[c][i];
        for(i = 0; i < 8; i++) OLED_GRAM[y/8+1][x+i] = F8X16[c][i+8];
    }
}

// 在显存中画字符串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size) {
    unsigned char j = 0;
    while (chr[j] != '\0') {		
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if(x > 120) { x = 0; y += 16; }
        j++;
    }
}

// 在显存中画图片 (修复了参数类型，与 oled.h 完全一致)
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, const uint8_t BMP[]) { 	
    uint16_t j = 0;
    uint8_t i, m;
    
    // 计算需要占据多少个 Page
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0); 
    
    for (i = 0; i < sizey; i++) {
        for (m = 0; m < sizex; m++) {
            OLED_GRAM[i + y][x + m] = BMP[j++]; // 直接写入显存
        }
    }
}

// OLED 初始化序列
void OLED_Init(void) {
    IIC_Init();       // 初始化底层的引脚
    delay_ms(200);    // 等待屏幕上电稳定

    Write_IIC_Command(0xAE); 
    Write_IIC_Command(0x00); 
    Write_IIC_Command(0x10); 
    Write_IIC_Command(0x40); 
    Write_IIC_Command(0x81); 
    Write_IIC_Command(0xCF); 
    Write_IIC_Command(0xA1); 
    Write_IIC_Command(0xC8); 
    Write_IIC_Command(0xA6); 
    Write_IIC_Command(0xA8); 
    Write_IIC_Command(0x3f); 
    Write_IIC_Command(0xD3); 
    Write_IIC_Command(0x00); 
    Write_IIC_Command(0xD5); 
    Write_IIC_Command(0x80); 
    Write_IIC_Command(0xD9); 
    Write_IIC_Command(0xF1); 
    Write_IIC_Command(0xDA); 
    Write_IIC_Command(0x12); 
    Write_IIC_Command(0xDB); 
    Write_IIC_Command(0x40); 
    Write_IIC_Command(0x20); 
    Write_IIC_Command(0x02); // 页寻址模式
    Write_IIC_Command(0x8D); 
    Write_IIC_Command(0x14); 
    Write_IIC_Command(0xA4); 
    Write_IIC_Command(0xA6); 
    Write_IIC_Command(0xAF); 
    
    OLED_Clear(); 
    OLED_Refresh_GRAM(); // 初始化后刷新一次纯黑显存
}