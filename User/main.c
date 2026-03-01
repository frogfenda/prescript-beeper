#include "stm32f10x.h"
#include "Sys_tik.h"
#include "prescript_logic.h" // 抽卡与特效引擎
#include "sleep_system.h"    // 休眠与待机管理

int main(void) {
    // 1. 系统与硬件底层初始化
    Prescript_Hardware_Init();
    
    // 2. 初始化休眠系统 (此时屏幕会画出待机Logo)
    Sleep_System_Init();

    // 局部状态变量：用于控制文字停留的时间
    uint16_t text_show_timer = 0; 

    while (1) {
        // --- 交互层：按键状态机 ---
        if (Is_Key_Pressed()) {
            delay_ms(20); // 软件消抖
            if (Is_Key_Pressed()) {
                while(Is_Key_Pressed()); // 等待松开
                
                // 逻辑分支：当前是在睡觉，还是醒着？
                if (Is_System_Sleeping()) {
                    Wake_Up_System();
                } else {
                    // 核心动作：狂暴抽取并展示指令
                    Prescript_Action();
                    
                    // 动作完成，开始5秒的文字展示倒计时
                    text_show_timer = 500; 
                    Reset_Idle_Timer();
                }
            }
        } 
        // --- 后台层：休眠时间与展示时间推演 ---
        else {
            delay_ms(10); // 提供 10ms 的基础时间片切分
            
            if (text_show_timer > 0) {
                // 此时正在展示文字，倒计时递减
                text_show_timer--;
                if (text_show_timer == 0) {
                    // 5秒结束，自动切回待机Logo，重新计算息屏时间
                    Restore_Standby_Logo(); 
                    Reset_Idle_Timer();
                }
            } else {
                // 此时处于待机Logo界面，开始累计并判断息屏时间
                Sleep_System_Tick();
            }
        }
    }
}