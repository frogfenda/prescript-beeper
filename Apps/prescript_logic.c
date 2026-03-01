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
    int max_jumps = 35;  
    
    for (int i = 0; i < len; i++) {
        if (target_str[i] == ' ') {
            display_buf[i] = ' ';
            lucky_jumps[i] = 0; 
        } else {
            display_buf[i] = ' ';
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
                display_buf[i] = 33 + (rand() % 94); 
                all_locked = 0; 
            }
        }

        OLED_ShowString(x, y, (uint8_t*)display_buf, 16); 
        if (!all_locked) Buzzer_Random_Glitch();
        delay_ms(1); 
        if (all_locked) break; 
        
        if (rand() % 100 < 5) { 
            Write_IIC_Command(0xA7); delay_ms(10); Write_IIC_Command(0xA6); 
        }
    }
    OLED_ShowString(x, y, (uint8_t*)target_str, 16);
}

// ================= 对外暴露的动作接口 =================
void Prescript_Action(void) {
    srand(Get_Random_Seed()); 
    int count = Get_Prescript_Count();
    int index = rand() % count;
    
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t*)"[ PRESCRIPT ]", 16);
    
    char final_prescript[80]; 
    sprintf(final_prescript, "_%s_", Get_Prescript(index));
    
    Prescript_Decode_Effect(final_prescript, 0, 16);
    
    delay_ms(200);
    BUZZER_ON();
    delay_ms(300);
    BUZZER_OFF();
}