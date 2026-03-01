#ifndef __SLEEP_SYSTEM_H
#define __SLEEP_SYSTEM_H

#include "stm32f10x.h"

// 初始化休眠系统（默认画上待机Logo）
void Sleep_System_Init(void);

// 获取当前是否在休眠
uint8_t Is_System_Sleeping(void);

// 唤醒系统（点亮屏幕并重绘Logo）
void Wake_Up_System(void);

// 强制进入休眠（息屏）
void Go_To_Sleep(void);

// 恢复待机Logo画面（但不改变唤醒状态）
void Restore_Standby_Logo(void);

// 重置闲置倒计时（每次有操作时调用）
void Reset_Idle_Timer(void);

// 休眠系统的心跳Tick（放在主循环里，每10ms调用一次计算闲置时间）
void Sleep_System_Tick(void);

#endif