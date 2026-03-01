#include "stm32f10x.h"
#include "app_fsm.h" // 引入你的高层状态机

int main(void) {
    // 1. 系统装载
    App_FSM_Init();

    // 2. 运转都市意志
    while (1) {
        App_FSM_Run();
    }
}