#ifndef __PRESCRIPT_LOGIC_H
#define __PRESCRIPT_LOGIC_H

#include "stm32f10x.h"

// 统一的底层硬件初始化
void Prescript_Hardware_Init(void);

// 提供给主循环的按键检测接口 (彻底让 main.c 摆脱 GPIO)
uint8_t Is_Key_Pressed(void);

// 执行一次完整的指令抽取与特效展示动作
void Prescript_Action(void);

#endif