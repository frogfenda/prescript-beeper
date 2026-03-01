#include "prescript_logic.h"
#include "prescript_data.h" 
#include "oled.h"    
#include "Sys_tik.h" // 确保你的精准延时头文件叫这个
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ================= 硬件引脚宏定义 =================
#define BUZZER_PIN    GPIO_Pin_8
#define BUZZER_PORT   GPIOA
#define KEY_PIN       GPIO_Pin_0
#define KEY_PORT      GPIOA

#define BUZZER_ON()   GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN) 
#define BUZZER_OFF()  GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)   
#define KEY_READ()    GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN)

// ================= 底层硬件打工模块 =================

void Prescript_Hardware_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    delay_init(); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    BUZZER_OFF(); 

    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    OLED_Init();        
    OLED_Clear();
}

uint8_t Is_Key_Pressed(void) {
    return (KEY_READ() == 0) ? 1 : 0;
}

static uint16_t Get_Random_Seed(void) {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); 
    return ADC_GetConversionValue(ADC1);
}

static void Buzzer_Random_Glitch(void) {
    int toggle_count = (rand() % 4) + 1; 
    int tone_delay_us = (rand() % 150) + 50; 
    
    for (int i = 0; i < toggle_count; i++) {
        BUZZER_ON();
        delay_us(tone_delay_us); 
        BUZZER_OFF();
        delay_us(tone_delay_us);
    }
}

static void Prescript_Decode_Effect(const char* target_str, uint8_t x, uint8_t y) {
    uint8_t len = strlen(target_str);
    char display_buf[80]; 
    int lucky_jumps[80]; 
    int max_jumps = 40;  
    
    for (int i = 0; i < len; i++) {
        if (target_str[i] == ' ') {
            display_buf[i] = ' ';
            lucky_jumps[i] = 0; 
        } else {
            display_buf[i] = ' ';
            if (i < 13) {
                // 抬头 [ PRESCRIPT ] 在极短时间内锁定
                lucky_jumps[i] = (rand() % 6) + 2; 
            } else {
                // 具体指令慢慢收束
                lucky_jumps[i] = (rand() % (max_jumps - 15)) + 15; 
            }
        }
    }
    display_buf[len] = '\0';

    // 【新增】：标志位，用于记录“抬头锁定音”是否已经播放过
    uint8_t header_locked_played = 0; 

    for (int frame = 0; frame < max_jumps; frame++) {
        uint8_t all_locked = 1; 
        
        // 【新增】：当前帧抬头是否已经全部锁定？默认认为是，下面遇到没锁定的再推翻
        uint8_t header_locked = 1; 

        for (int i = 0; i < len; i++) {
            if (target_str[i] == ' ') continue;

            if (frame >= lucky_jumps[i]) {
                display_buf[i] = target_str[i];
            } else {
                display_buf[i] = 33 + (rand() % 94); 
                all_locked = 0; 
                // 如果这个没锁定的字符属于前13个（即抬头部分），说明抬头还没完全锁定
                if (i < 13) header_locked = 0; 
            }
        }

        OLED_ShowString(x, y, (uint8_t*)display_buf, 16); 

        // 【核心视听反馈】：在抬头刚好全部锁定的那一瞬间，发出一声清脆短促的“滴”！
        if (header_locked && !header_locked_played) {
            BUZZER_ON();
            delay_ms(40); // 40ms的长鸣，与几微秒的运算碎音形成鲜明对比
            BUZZER_OFF();
            header_locked_played = 1; // 标记已响，之后不再重复触发
        }

        // 如果还没全部解密完，继续发出滋滋的碎音
        if (!all_locked) {
            Buzzer_Random_Glitch();
        }

        delay_ms(1); 

        if (all_locked) {
            break; 
        }
        
        if (rand() % 100 < 5) { 
            Write_IIC_Command(0xA7); 
            delay_ms(10);            
            Write_IIC_Command(0xA6); 
        }
    }
    
    OLED_ShowString(x, y, (uint8_t*)target_str, 16);
}
// ================= 对外暴露的动作接口 =================
// ================= 对外暴露的动作接口 =================
// ================= 对外暴露的动作接口 =================
void Prescript_Action(void) {
    srand(Get_Random_Seed()); 
    
    // ---------------------------------------------------------
    // 【阶段 1：全屏混沌乱码】
    // ---------------------------------------------------------
    OLED_Clear();
    char row_buf[17];  
    row_buf[16] = '\0';
    uint16_t chaos_timeout = 0;
    
    while(Is_Key_Pressed());
    delay_ms(20);
    
    // 等待确认：4 行全部陷入疯狂乱码
    while(!Is_Key_Pressed()) {
        for(int row = 0; row < 4; row++) {
            for(int i = 0; i < 16; i++) {
                row_buf[i] = 33 + (rand() % 94); // 生成可视乱码
            }
            OLED_ShowString(0, row * 16, (uint8_t*)row_buf, 16);
        }
        
        Buzzer_Random_Glitch();
        delay_ms(5); 
        
        chaos_timeout++;
        if(chaos_timeout > 3000) break; 
    }
    
    // ---------------------------------------------------------
    // 【阶段 2：信号锁定，无缝解密】
    // ---------------------------------------------------------
    while(Is_Key_Pressed()); 
    delay_ms(20);

    int count = Get_Prescript_Count();
    int index = rand() % count;
    
    char final_prescript[80]; 
    
    // 魔法拼接：[ PRESCRIPT ] 占13字符，加3个空格刚好挤满第一行！第二行开始是具体指令。
    sprintf(final_prescript, "[ PRESCRIPT ]   _%s_", Get_Prescript(index));
    
    // 核心降噪逻辑：把剩余的屏幕空间全部用空格填满（总共64个字符即全屏）
    int len = strlen(final_prescript);
    for(int i = len; i < 64; i++) {
        final_prescript[i] = ' ';
			
    }
    final_prescript[64] = '\0'; // 加上字符串结束符
    
    // 绝对不能清屏！直接从 (0, 0) 坐标开始全屏覆盖解密！
    Prescript_Decode_Effect(final_prescript, 0, 0);
    
    // 宣告命运
    delay_ms(200);
    BUZZER_ON();
    delay_ms(300);
    BUZZER_OFF();
}