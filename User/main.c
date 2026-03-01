
#include "oled.h"    // 确保你的 oled.h 里已经引入了 my_iic.h
#include "main.h"   // 确保你有标准的正点原子或野火的 delay_ms 函数
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // 需要用到 sprintf
#include "bmp.h"

// ================= 硬件引脚宏定义 =================

#define BUZZER_PIN    GPIO_Pin_8
#define BUZZER_PORT   GPIOA
#define KEY_PIN       GPIO_Pin_0
#define KEY_PORT      GPIOA

#define BUZZER_ON()   GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN) 
#define BUZZER_OFF()  GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)   
#define KEY_READ()    GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN)

// ================= 扩充版食指指令库 =================
// 警告：单条指令（含空格和标点）长度不得超过 46！
const char* prescripts[] = {
    "STARE AT A CORNER FOR EXACTLY 300 SECONDS.",    // 42字符
    "TEAR A BLANK PAGE INTO EXACTLY 17 PIECES.",     // 41字符
    "BITE YOUR LEFT INDEX FINGER UNTIL IT HURTS.",   // 43字符
    "WHISPER 'YES' TO THE NEXT PERSON WHO SPEAKS.",  // 44字符
    "POUR A GLASS OF WATER AND LET IT EVAPORATE.",   // 43字符
    "TIE A BLACK THREAD AROUND YOUR RIGHT WRIST.",   // 43字符
    "WALK BACKWARDS FOR YOUR NEXT 20 STEPS.",        // 38字符
    "APOLOGIZE TO THE NEXT DEAD INSECT YOU FIND.",   // 43字符
    "LEAVE A RUSTY COIN ON THE THIRD STAIR.",        // 38字符
    "DO NOT BLINK WHEN YOU CROSS THE NEXT DOOR.",    // 42字符
    "THROW A KEY INTO THE NEAREST RIVER.",           // 35字符
    "HOLD YOUR BREATH UNTIL YOU HEAR A SIREN." ,      // 40字符
		"FIND ISHMALE",
		"KILL YOUR PAINTING",
		"CLEAR",
		"I'm BUGRER"
};
#define PRESCRIPT_COUNT (sizeof(prescripts) / sizeof(prescripts[0]))

// 自动计算新的指令总数
#define PRESCRIPT_COUNT (sizeof(prescripts) / sizeof(prescripts[0]))

// ================= 硬件初始化模块 =================

void Hardware_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 1. 开启时钟：GPIOA 和 ADC1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // 2. 初始化蜂鸣器 (PA8 推挽输出)
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    BUZZER_OFF(); // 默认关闭不响

    // 3. 初始化按键 (PA0 上拉输入，假设按下接地)
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

    // 4. 初始化 ADC (PA1 模拟输入，什么都不要接，用于读取空间噪声)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC 基础配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // 使能并校准 ADC
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

// 获取ADC悬空引脚的噪声值作为真·随机种子
uint16_t Get_Random_Seed(void) {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); // 等待转换结束
    return ADC_GetConversionValue(ADC1);
}
// 制造频率随机、极短促的“赛博运算”碎音 (现在可以用纯正的硬件延时了！)
void Buzzer_Random_Glitch(void) {
    int toggle_count = (rand() % 6) + 3; 
    int tone_delay_us = (rand() % 200) + 50; 
    
    for (int i = 0; i < toggle_count; i++) {
        BUZZER_ON();
        delay_us(tone_delay_us); // 使用 SysTick 提供的完美 1us 精度延时！
        BUZZER_OFF();
        delay_us(tone_delay_us);
    }
}
// ================= 食指指令解密核心特效 =================



void Prescript_Decode_Effect(const char* target_str, uint8_t x, uint8_t y) {
    uint8_t len = strlen(target_str);
    
    // 【修改点3】：将显存和命运节点数组全部放大到 80，绰绰有余
    char display_buf[80]; 
    int lucky_jumps[80]; 
    
    int max_jumps = 35;  // 稍微加长了一点点最大解密时间，让长句解密更有观赏性
    
    for (int i = 0; i < len; i++) {
        if (target_str[i] == ' ') {
            display_buf[i] = ' ';
            lucky_jumps[i] = 0; 
        } else {
            display_buf[i] = ' ';
            // 给每个字母分配一个 5 到 max_jumps 之间的随机命运节点
            lucky_jumps[i] = (rand() % (max_jumps - 5)) + 5; 
        }
    }
    display_buf[len] = '\0';

    for (int frame = 0; frame < max_jumps; frame++) {
        uint8_t all_locked = 1; 

        for (int i = 0; i < len; i++) {
            if (target_str[i] == ' ') continue;

            if (frame >= lucky_jumps[i]) {
                display_buf[i] = target_str[i];
            } else {
                // 未锁定的位置继续疯狂闪烁
                display_buf[i] = 33 + (rand() % 94); 
                all_locked = 0; 
            }
        }

        // 调用自带自动换行功能的 ShowString 刷新整个多行乱码
        OLED_ShowString(x, y, (uint8_t*)display_buf, 16); 

        if (!all_locked) {
							Buzzer_Random_Glitch();
        }
        


        if (all_locked) {
            break; 
        }
				if (rand() % 100 < 5) { // 5% 的概率产生一次视觉撕裂
					Write_IIC_Command(0xA7); // 屏幕反色
					delay_ms(10);             // 极短时间闪烁
					Write_IIC_Command(0xA6); // 恢复正常
}
    }
    
    strcpy(display_buf, target_str);
    OLED_ShowString(x, y, (uint8_t*)display_buf, 16);
}
// ================= 主函数 =================

// ... 前面的指令库和特效函数保持不变 ...

int main(void) {
    // 系统底层初始化
    delay_init();     
    Hardware_Init();    
    OLED_Init();        
    OLED_Clear();

    // 状态机变量
    uint16_t idle_timer = 0;      // 闲置息屏计时器
    uint16_t show_text_timer = 0; // 【新增】文字展示倒计时器
    uint8_t is_sleeping = 0;      // 是否处于息屏睡眠状态
    
    // 启动时先显示食指的待机 Logo
    OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo);

    while (1) {
        // 检测按键是否按下
        if (KEY_READ() == 0) {
            delay_ms(20); // 软件消抖
            if (KEY_READ() == 0) {
                while(KEY_READ() == 0); // 等待释放
                
                if (is_sleeping) {
                    // 状态1：从休眠中唤醒
                    Write_IIC_Command(0xAF); 
                    OLED_Clear();
                    OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo); 
                    is_sleeping = 0; 
                    idle_timer = 0;  
                } 
                else {
                    // 状态2：亮屏状态下按下按键（无论是待机界面，还是指令展示界面，都能立刻打断！）
                    srand(Get_Random_Seed()); 
                    int index = rand() % PRESCRIPT_COUNT;
                    
                    OLED_Clear();
						
										OLED_Clear();
                    OLED_ShowString(0, 0, (uint8_t*)"[ PRESCRIPT ]", 16);
                    
                    char final_prescript[80]; 
                    sprintf(final_prescript, "_%s_", prescripts[index]);
										
                    Prescript_Decode_Effect(final_prescript, 0, 16);
                    
                    delay_ms(200);
                    BUZZER_ON();
                    delay_ms(300);
                    BUZZER_OFF();
                    
                    // 【核心修复】：不再使用死等！设置一个倒计时标志后，立刻回到主循环！
                    show_text_timer = 500; // 500 * 10ms = 5秒展示时间
                    idle_timer = 0;        // 重置闲置计时器
                }
            }
        } 
        else {
            // 没有按键按下时的后台处理
            if (!is_sleeping) {
                delay_ms(10); // 每次循环的基础时间片 10ms
                
                // 状态分流：当前屏幕上是在展示指令文字，还是待机 Logo？
                if (show_text_timer > 0) {
                    // 正在展示指令文字，倒计时递减
                    show_text_timer--;
                    if (show_text_timer == 0) {
                        // 5秒倒计时结束，自动切回待机 Logo
                        OLED_Clear();
                        OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo);
                        idle_timer = 0; // 重新开始计算息屏时间
                    }
                } 
                else {
                    // 当前显示的是待机 Logo，累计闲置时间准备息屏
                    idle_timer++;
                    if (idle_timer > 10000) { // 1000 * 10ms = 10秒
                        OLED_Clear();
                        Write_IIC_Command(0xAE); // 息屏
                        is_sleeping = 1;
                    }
                }
            }
        }
    }
}