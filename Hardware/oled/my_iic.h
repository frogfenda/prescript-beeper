#ifndef __MY_IIC_H
#define __MY_IIC_H

#include "stm32f10x.h"

// ================= 引脚宏定义 =================
#define IIC_SCL_PORT GPIOB
#define IIC_SCL_PIN  GPIO_Pin_8
#define IIC_SDA_PORT GPIOB
#define IIC_SDA_PIN  GPIO_Pin_9

// ================= IIC 状态控制宏 =================
// 强行写入寄存器，速度比调用库函数快，保证 IIC 通信速率
#define IIC_SCL(x)  if(x) GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN); \
                    else  GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN)

#define IIC_SDA(x)  if(x) GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN); \
                    else  GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN)

// 读取 SDA 线上的电平状态
#define IIC_SDA_READ()  GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN)

// ================= 函数声明 =================
void IIC_Init(void);                // 初始化IIC引脚
void IIC_Start(void);               // 发送IIC开始信号
void IIC_Stop(void);                // 发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);    // IIC发送一个字节
uint8_t IIC_Wait_Ack(void);         // IIC等待应答

#endif