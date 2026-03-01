#include "app_fsm.h"
#include "prescript_logic.h"
#include "sleep_system.h"
#include "Sys_tik.h" // 或者是 Sys_tik.h 里的延时函数

// ================= 定义系统的 3 个绝对状态 =================
typedef enum {
    STATE_SLEEP = 0,       // 状态 0：深度休眠（息屏）
    STATE_STANDBY,         // 状态 1：待机监控（显示 Logo）
    STATE_SHOWING_TEXT     // 状态 2：指令展示（显示文字）
} SystemState;

static SystemState current_state = STATE_STANDBY;
static uint16_t text_show_timer = 0;

void App_FSM_Init(void) {
    Prescript_Hardware_Init();
    Sleep_System_Init();
    current_state = STATE_STANDBY;
}

void App_FSM_Run(void) {
    // ---------------------------------------------------------
    // 【交互层】：按键事件接管
    // ---------------------------------------------------------
    if (Is_Key_Pressed()) {
        delay_ms(20); // 软件消抖
        if (Is_Key_Pressed()) {
            while(Is_Key_Pressed()); // 等待按键彻底松开
            
            if (current_state == STATE_SLEEP) {
                // 睡觉时被按：只唤醒，重回待机状态
                Wake_Up_System();
                current_state = STATE_STANDBY;
                Reset_Idle_Timer();
            } else {
                // 醒着时被按：无论是 Logo 还是正在展示上一条文字，直接强制进入抽卡
                Prescript_Action(); 
                
                // 【核心逻辑】：动作引擎是阻塞的，到这里说明“文字已经完全解密并显示完毕”
                // 此时进入文字展示状态，并重置文字停留倒计时 (500 * 10ms = 5秒)
                current_state = STATE_SHOWING_TEXT;
                text_show_timer = 500; 
            }
        }
    } 
    // ---------------------------------------------------------
    // 【后台层】：时间推演与状态流转
    // ---------------------------------------------------------
    else {
        delay_ms(10); // 提供 10ms 的基础时间片
        
        switch (current_state) {
            
            case STATE_SHOWING_TEXT:
                // 在此状态下，闲置倒计时 (idle_timer) 是处于冻结状态的！
                if (text_show_timer > 0) {
                    text_show_timer--;
                } else {
                    // 文字 5 秒展示结束，切回待机 Logo
                    Restore_Standby_Logo();
                    current_state = STATE_STANDBY;
                    
                    // 【满足你的需求】：直到切回待机界面，才把息屏计时器清零，开始计算 10 秒息屏！
                    Reset_Idle_Timer(); 
                }
                break;
                
            case STATE_STANDBY:
                // 只有处于 Logo 待机状态，才会执行息屏倒计时
                Sleep_System_Tick();
                
                // 检查底层休眠系统是否已经把屏幕关了
                if (Is_System_Sleeping()) {
                    current_state = STATE_SLEEP; // 状态机同步切入睡眠状态
                }
                break;
                
            case STATE_SLEEP:
                // 睡觉状态下时间静止，什么也不做，等待按键打破循环
                break;
        }
    }
}