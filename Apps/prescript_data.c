#include "prescript_data.h"

// =========================================================================
//                           [ 食 指 指 令 库 ]
// 复刻者请注意：
// 1. 可以在这里随意添加、删除或修改你的专属指令。
// 2. 单条指令（含空格和标点）长度建议不超过 46 个字符，否则 OLED 屏幕会换行溢出。
// =========================================================================

static const char* prescripts[] = {
    "STARE AT A CORNER FOR EXACTLY 300 SECONDS.",    
    "TEAR A BLANK PAGE INTO EXACTLY 17 PIECES.",     
    "BITE YOUR LEFT INDEX FINGER UNTIL IT HURTS.",   
    "WHISPER 'YES' TO THE NEXT PERSON WHO SPEAKS.",  
    "POUR A GLASS OF WATER AND LET IT EVAPORATE.",   
    "TIE A BLACK THREAD AROUND YOUR RIGHT WRIST.",   
    "WALK BACKWARDS FOR YOUR NEXT 20 STEPS.",        
    "APOLOGIZE TO THE NEXT DEAD INSECT YOU FIND.",   
    "LEAVE A RUSTY COIN ON THE THIRD STAIR.",        
    "DO NOT BLINK WHEN YOU CROSS THE NEXT DOOR.",    
    "THROW A KEY INTO THE NEAREST RIVER.",           
    "HOLD YOUR BREATH UNTIL YOU HEAR A SIREN." ,      
    "FIND ISHMAEL",
    "KILL YOUR PAINTING",
    "CLEAR",
    "I'm BURGER"
};

// 自动计算当前的指令总数
static const int PRESCRIPT_COUNT = sizeof(prescripts) / sizeof(prescripts[0]);

// ================= 对外提供的访问接口 =================

int Get_Prescript_Count(void) {
    return PRESCRIPT_COUNT;
}

const char* Get_Prescript(int index) {
    // 增加一个安全防护，防止随机数越界导致死机
    if (index >= 0 && index < PRESCRIPT_COUNT) {
        return prescripts[index];
    }
    return "ERROR: PRESCRIPT NOT FOUND."; 
}