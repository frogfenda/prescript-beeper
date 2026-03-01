#include "main.h"
#include "Sys_tik.h"


// 静态变量，用来保存延时乘数
static uint8_t  fac_us = 0; // us延时倍乘数
static uint16_t fac_ms = 0; // ms延时倍乘数

// 初始化延迟函数
// SYSTICK 的时钟固定为 HCLK 时钟的 1/8 (72MHz / 8 = 9MHz)
void delay_init(void) {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 选择外部时钟
    fac_us = SystemCoreClock / 8000000;                   // 为 9 (9个SysTick时钟周期就是1微秒)
    fac_ms = (uint16_t)fac_us * 1000;                     // 为 9000
}

// 微秒级精确延时
// nus: 要延时的微秒数 (0~1864135)
void delay_us(uint32_t nus) {		
    uint32_t temp;	    	 
    SysTick->LOAD = nus * fac_us;             // 加载时间 (设置计数器起点)
    SysTick->VAL = 0x00;                      // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数	  
    
    // 循环死等，直到 SysTick 计数器倒数到 0 (寄存器的第16位 COUNTFLAG 变1)
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));  
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭计数器
    SysTick->VAL = 0X00;                       // 清空计数器	 
}

// 毫秒级精确延时
// nms: 要延时的毫秒数 (0~1864)
void delay_ms(uint16_t nms) {	 		  	  
    uint32_t temp;		   
    SysTick->LOAD = (uint32_t)nms * fac_ms;   // 加载时间
    SysTick->VAL = 0x00;                      // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
    
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭计数器
    SysTick->VAL = 0X00;                       // 清空计数器	  	    
}