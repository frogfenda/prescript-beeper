#include "my_iic.h"

// IIC微秒级延时函数 (由于系统SysTick的delay_us在不同时钟下可能不准)
// 这里用空循环代替，足够应付OLED的400kHz IIC速率
static void IIC_Delay(void) {
    uint8_t i = 5; 
    while(i--); 
}

// IIC初始化
void IIC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 开启 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置 PB8(SCL) 和 PB9(SDA)
    // 核心点：使用开漏输出(Out_OD)。开漏模式下，输出1时引脚浮空，
    // 此时依靠OLED模块自带的上拉电阻拉高，这样就可以直接读取引脚电平。
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN | IIC_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化时总线释放，全部拉高
    IIC_SCL(1);
    IIC_SDA(1);
}

// 产生IIC起始信号
void IIC_Start(void) {
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(0); // SCL为高时，SDA由高变低表示起始
    IIC_Delay();
    IIC_SCL(0); // 钳住I2C总线，准备发送或接收数据 
}

// 产生IIC停止信号
void IIC_Stop(void) {
    IIC_SCL(0);
    IIC_SDA(0); 
    IIC_Delay();
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(1); // SCL为高时，SDA由低变高表示停止
    IIC_Delay();
}

// 等待应答信号到来
// 返回值：1，接收应答失败；0，接收应答成功
uint8_t IIC_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    
    IIC_SDA(1); // 释放SDA线，等待从机(OLED)拉低
    IIC_Delay();
    IIC_SCL(1);
    IIC_Delay();
    
    // 等待从机将SDA拉低
    while(IIC_SDA_READ()) {
        ucErrTime++;
        if(ucErrTime > 250) {
            IIC_Stop();
            return 1; // 超时，无应答
        }
    }
    IIC_SCL(0); // 时钟输出0
    return 0;   // 成功收到应答
}

// IIC发送一个字节
void IIC_Send_Byte(uint8_t txd) {                        
    uint8_t t;   
    IIC_SCL(0); // 拉低时钟开始数据传输
    for(t = 0; t < 8; t++) {
        // 从最高位(MSB)开始发送
        if((txd & 0x80) >> 7) {
            IIC_SDA(1);
        } else {
            IIC_SDA(0);
        }
        txd <<= 1;
        IIC_Delay(); // 等待数据稳定
        
        IIC_SCL(1);  // 拉高时钟，让从机读取数据
        IIC_Delay();
        IIC_SCL(0);  // 拉低时钟，准备下一位数据
        IIC_Delay();
    }
}