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
		OLED_Refresh_GRAM();
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


// ================= 新增：多音阶音效引擎 =================

// 基础发声函数：指定频率(Hz)和时长(ms)
static void Buzzer_Play_Tone(uint16_t freq_hz, uint16_t duration_ms) {
    if (freq_hz == 0) {
        delay_ms(duration_ms); // 如果频率为0，当作休止符（静音）
        return;
    }
    // 计算半周期的微秒数 (T = 1/f, 半周期 = 1000000 / f / 2)
    uint32_t half_period_us = 500000 / freq_hz; 
    // 计算在这段时间内需要震动多少次
    uint32_t cycles = (duration_ms * 1000) / (half_period_us * 2); 
    
    for (uint32_t i = 0; i < cycles; i++) {
        BUZZER_ON();
        delay_us(half_period_us);
        BUZZER_OFF();
        delay_us(half_period_us);
    }
}
static void Buzzer_Random_Glitch(void) {
    // 【细节调优】：加入 20% 的“哑火”概率。
    // 这种偶尔空拍一两毫秒的停顿，能极大增加运算卡顿、破损的机械感！
    if (rand() % 100 < 5) {
        delay_ms(1);
        return;
    }
    
    // 随机生成 2000Hz 到 8000Hz 之间的高频“电火花/运算”杂音
    uint16_t random_freq = (rand() % 6000) + 2000; 
    
    // 随机生成 1ms 到 3ms 的极短促时间 (模拟原本 toggles 的碎裂感)
    uint16_t random_duration = (rand() % 5) + 2;   
    
    // 直接调用我们的统一多音阶引擎！
    Buzzer_Play_Tone(random_freq, random_duration);
}
// 1. 抬头锁定音：急促的“低-高”双音（充满系统确认的科技感）
static void Sound_Header_Locked(void) {
    Buzzer_Play_Tone(1500, 30); // 较沉闷的 1500Hz 响 30ms
    Buzzer_Play_Tone(2500, 40); // 极清脆的 2500Hz 响 40ms
}

// 2. 解析完成音：庄重的“滴-滴-叮~”（三音阶升调宣告）
static void Sound_Decode_Complete(void) {
    Buzzer_Play_Tone(1200, 60);  // 第一声低沉
    delay_ms(20);                // 短暂的呼吸停顿
    Buzzer_Play_Tone(1200, 60);  // 第二声拉高
    delay_ms(20);                
    Buzzer_Play_Tone(1200, 60); // 最后一声嘹亮的长鸣宣告
		delay_ms(20);                
    Buzzer_Play_Tone(1200, 60); // 最后一声嘹亮的长鸣宣告
}
// ================= 新增：单行快速解密引擎 (用于滚动时底部的文字) =================
static void Prescript_Decode_Line(const char* target_str, uint8_t x, uint8_t y) {
    uint8_t len = strlen(target_str);
    char display_buf[20]; 
    int lucky_jumps[20]; 
    int max_jumps = 20; // 滚动时的解密可以干脆利落一点

    for (int i = 0; i < len; i++) {
        if (target_str[i] == ' ') {
            display_buf[i] = ' ';
            lucky_jumps[i] = 0; 
        } else {
            display_buf[i] = ' ';
            lucky_jumps[i] = (rand() % 10) + 5; 
        }
    }
    display_buf[len] = '\0';

    for (int frame = 0; frame < max_jumps; frame++) {
        uint8_t all_locked = 1;
        for (int i = 0; i < len; i++) {
            if (target_str[i] == ' ') continue;
            if (frame >= lucky_jumps[i]) display_buf[i] = target_str[i];
            else { display_buf[i] = 33 + (rand() % 94); all_locked = 0; }
        }

        OLED_ShowString(x, y, (uint8_t*)display_buf, 16); 
        OLED_Refresh_GRAM();

        if (!all_locked) Buzzer_Random_Glitch();
        if (all_locked) break; 
        
    }
    OLED_ShowString(x, y, (uint8_t*)target_str, 16);
    OLED_Refresh_GRAM();
}

static void Prescript_Decode_Effect(const char* target_str, uint8_t x, uint8_t y) {
    uint8_t len = strlen(target_str);
    char display_buf[80]; 
    int lucky_jumps[80]; 
    
    // 【优化 1】：把最大跳动帧数从 40 压缩到 25 帧，大幅缩短整体拖沓感
    int max_jumps = 25;  
    
    for (int i = 0; i < len; i++) {
        if (target_str[i] == ' ') {
            display_buf[i] = ' ';
            lucky_jumps[i] = 0; 
        } else {
            display_buf[i] = ' ';
            if (i < 13) {
                // 抬头 [ PRESCRIPT ] 在第 1~4 帧之间极速锁定
                lucky_jumps[i] = (rand() % 4) + 1; 
            } else {
                // 【优化 2】：下方指令在第 8~22 帧之间收束（原来是 15~40帧）
                // 这样既保留了抬头先锁定的层次感，又让下方指令的解密变得干脆利落！
                lucky_jumps[i] = (rand() % 16) + 8; 
            }
        }
    }
    display_buf[len] = '\0';

    uint8_t header_locked_played = 0; 

    for (int frame = 0; frame < max_jumps; frame++) {
        uint8_t all_locked = 1; 
        uint8_t header_locked = 1; 

        for (int i = 0; i < len; i++) {
            if (target_str[i] == ' ') continue;

            if (frame >= lucky_jumps[i]) {
                display_buf[i] = target_str[i];
            } else {
                display_buf[i] = 33 + (rand() % 94); 
                all_locked = 0; 
                if (i < 13) header_locked = 0; 
            }
        }

        OLED_ShowString(x, y, (uint8_t*)display_buf, 16); 
				OLED_Refresh_GRAM();

        if (header_locked && !header_locked_played) {
						Sound_Header_Locked();
            header_locked_played = 1; 
        }

        if (!all_locked) {
            Buzzer_Random_Glitch();
        }

        // 【优化 3】：删掉了这里的 delay_ms(1);
        // 因为单片机刷新满屏 64 个字符的 IIC 通信本身就已经自带了几十毫秒的物理延时，不需要再人为拖慢了。

        if (all_locked) {
            break; 
        }
        

    }
    
    OLED_ShowString(x, y, (uint8_t*)target_str, 16);
}
// ================= 对外暴露的动作接口 =================

// ================= 对外暴露的动作接口 =================
// =========================================================================
// [ 引擎 1：混沌待机引擎 ] 负责全屏乱码与按键等待
// =========================================================================
static void Engine_Chaos_Wait(void) {
    OLED_Clear();
    char row_buf[17];  
    row_buf[16] = '\0';
    uint16_t chaos_timeout = 0;
    
    while(Is_Key_Pressed()); // 等待上一次按键彻底松开
    delay_ms(20);
    
    // 陷入疯狂乱码，直到代行者再次按下按钮确认
    while(!Is_Key_Pressed()) {
        for(int row = 0; row < 4; row++) {
            for(int i = 0; i < 16; i++) row_buf[i] = 33 + (rand() % 94); 
            OLED_ShowString(0, row * 16, (uint8_t*)row_buf, 16);
        }
        OLED_Refresh_GRAM(); 
        Buzzer_Random_Glitch();
        delay_ms(5); 
        
        if(++chaos_timeout > 3000) break; // 超时保护
    }
    
    while(Is_Key_Pressed()); // 确认后等待按键抬起
    delay_ms(20);
}

// =========================================================================
// [ 引擎 2：智能排版引擎 ] 负责单词防断行 (Word-Wrap) 算法
// =========================================================================
static void Engine_Format_Text(const char* raw_str, char* formatted_buf, int* out_len) {
    int total_src_len = strlen(raw_str);
    sprintf(formatted_buf, "[ PRESCRIPT ]   "); // 强行写入抬头
    int fmt_len = 16; 
    int src_idx = 0;

    while (src_idx < total_src_len) {
        while (raw_str[src_idx] == ' ') src_idx++; // 过滤句首多余空格
        if (src_idx >= total_src_len) break;

        int chunk = total_src_len - src_idx;
        if (chunk > 16) {
            if (raw_str[src_idx + 16] == ' ' || raw_str[src_idx + 16] == '\0') {
                chunk = 16;
            } else {
                int space_idx = -1;
                for (int i = 15; i > 0; i--) {
                    if (raw_str[src_idx + i] == ' ') { space_idx = i; break; }
                }
                chunk = (space_idx != -1) ? space_idx : 16; 
            }
        }

        for (int i = 0; i < 16; i++) {
            formatted_buf[fmt_len++] = (i < chunk) ? raw_str[src_idx + i] : ' ';
        }
        src_idx += chunk; 
    }
    formatted_buf[fmt_len] = '\0'; 
    *out_len = fmt_len; // 导出排版后的总长度
}

// =========================================================================
// [ 引擎 3：命运卷轴引擎 ] 负责显存搬运与逐行滚动解密
// =========================================================================
static void Engine_Scroll_Text(const char* formatted_buf, int fmt_len) {
    if (fmt_len <= 64) return; // 不足 4 行不需要滚动

    int current_idx = 64; 
    while (current_idx < fmt_len) {
        
        // 分层停顿逻辑
        if (current_idx == 64) {
            delay_ms(1000); delay_ms(1000); // 首屏读 2 秒
        } else {
            delay_ms(600);                  // 后续滚 0.6 秒
        }
        
        // 显存画面上移 2 页 (16像素)
        for (int p = 2; p < 6; p++) {
            for (int c = 0; c < 128; c++) OLED_GRAM[p][c] = OLED_GRAM[p+2][c];
        }
        // 清理底部两页留出空白
        for (int p = 6; p < 8; p++) {
            for (int c = 0; c < 128; c++) OLED_GRAM[p][c] = 0x00;
        }
        
        OLED_Refresh_GRAM();
        Buzzer_Play_Tone(3000, 30); // 机械咔哒声
        
        // 提取新的一行并解密
        char line_buf[17];
        strncpy(line_buf, &formatted_buf[current_idx], 16);
        line_buf[16] = '\0';
        Prescript_Decode_Line(line_buf, 0, 48);
        
        current_idx += 16;
    }
}
// =========================================================================
// [ 总指挥 ] 对外暴露的业务动作接口
// =========================================================================
void Prescript_Action(void) {
    // 0. 更新命运之种
    srand(Get_Random_Seed()); 
    
    // 1. 进入待命混沌状态
    Engine_Chaos_Wait();
    
    // 2. 抽取指令并进行智能排版
    int index = rand() % Get_Prescript_Count();
    char raw_prescript[256]; 
    sprintf(raw_prescript, "_%s_", Get_Prescript(index));
    
    char formatted_buf[512]; 
    int fmt_len = 0;
    Engine_Format_Text(raw_prescript, formatted_buf, &fmt_len);
    
    // 3. 渲染首屏 (前 64 个字符)
    char first_screen_buf[65]; 
    int first_chunk = (fmt_len > 64) ? 64 : fmt_len;
    strncpy(first_screen_buf, formatted_buf, first_chunk);
    for(int i = first_chunk; i < 64; i++) first_screen_buf[i] = ' '; // 补齐空格盖住乱码
    first_screen_buf[64] = '\0'; 
    
    Prescript_Decode_Effect(first_screen_buf, 0, 0);
    
    // 4. 启动长文本卷轴引擎 (如果需要)
    Engine_Scroll_Text(formatted_buf, fmt_len);
    
    // 5. 宣告绝对意志
    delay_ms(200);
    Sound_Decode_Complete();
}