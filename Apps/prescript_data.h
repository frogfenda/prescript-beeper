#ifndef __PRESCRIPT_DATA_H
#define __PRESCRIPT_DATA_H

// 获取当前指令库中一共有多少条指令
int Get_Prescript_Count(void);

// 根据随机抽取的序号，获取对应的指令字符串
const char* Get_Prescript(int index);

#endif