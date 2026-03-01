#ifndef __APP_FSM_H
#define __APP_FSM_H

#include "stm32f10x.h"

// 状态机初始化
void App_FSM_Init(void);

// 状态机主循环（放在 main 的 while(1) 里）
void App_FSM_Run(void);

#endif