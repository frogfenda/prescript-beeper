#include "sleep_system.h"
#include "oled.h"
#include "bmp.h"

static uint16_t idle_timer = 0;      
static uint8_t is_sleeping = 0;      

void Sleep_System_Init(void) {
    is_sleeping = 0;
    idle_timer = 0;
    OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo);
		OLED_Refresh_GRAM();
}

uint8_t Is_System_Sleeping(void) {
    return is_sleeping;
}

void Wake_Up_System(void) {
    Write_IIC_Command(0xAF); // 唤醒 OLED 显示
    OLED_Clear();
    OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo);
		OLED_Refresh_GRAM();
    is_sleeping = 0;
    idle_timer = 0;
}

void Go_To_Sleep(void) {
    OLED_Clear();
		OLED_Refresh_GRAM();
    Write_IIC_Command(0xAE); // 息屏指令
    is_sleeping = 1;
}

void Restore_Standby_Logo(void) {
    OLED_Clear();
    OLED_DrawBMP(0, 0, 128, 64, BMP_Index_Logo);
		OLED_Refresh_GRAM();
}

void Reset_Idle_Timer(void) {
    idle_timer = 0;
}

void Sleep_System_Tick(void) {
    if (!is_sleeping) {
        idle_timer++;
        // 闲置超过 10 秒 (1000 * 10ms) 触发息屏
        if (idle_timer > 10000) { 
            Go_To_Sleep();
        }
    }
}