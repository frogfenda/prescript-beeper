#include "prescript_data.h"

// =========================================================================
//                           [ 食 指 指 令 库 ]
// 复刻者请注意：
// 1. 可以在这里随意添加、删除或修改你的专属指令。
// 2. 单条指令（含空格和标点）长度建议不超过 46 个字符，否则 OLED 屏幕会换行溢出。
// =========================================================================

// =========================================================================
//                           [ 食 指 指 令 库 V3.0 ]
// =========================================================================

static const char* prescripts[] = {
    // === [ 经典短指令：干脆利落的命运判定 ] ===
    "FIND ISHMAEL.",
    "KILL YOUR PAINTING.",
    "CLEAR.",
    "I'M BURGER.",
    "STARE AT A CORNER FOR EXACTLY 300 SECONDS.",    
    "TEAR A BLANK PAGE INTO EXACTLY 17 PIECES.",     
    "WHISPER 'YES' TO THE NEXT PERSON WHO SPEAKS.",  
    "LEAVE A RUSTY COIN ON THE THIRD STAIR.",        
    "DO NOT BLINK WHEN YOU CROSS THE NEXT DOOR.",    

    // === [ 手工与刻印：留痕的仪式 ] ===
    "CARVE A SMILE INTO A RUBBER BLOCK AND BURN IT.",
    "CARVE THE EXACT SHAPE OF YOUR THUMBPRINT ONTO A FRESH RUBBER BLOCK. ONCE COMPLETED, COAT IT WITH BLACK INK AND STAMP IT ON THE NEAREST CONCRETE WALL UNTIL THE INK RUNS DRY.",
    "PRESS RED INK ONTO A BLANK PAGE FOREVER.",

    // === [ 电子与工业：废弃终端的低语 ] ===
    "CUT THE RED WIRE. LEAVE THE BLUE WIRE.",
    "SOLDER EXACTLY 14 COPPER WIRES INTO A PERFECT WEB. CONNECT THE CENTER TO GROUND, AND LET THE ENDS HANG LOOSE. LISTEN TO THE STATIC UNTIL IT SOUNDS LIKE A VOICE.",
    "COUNT THE PINS ON A DEAD MICROCONTROLLER.",
    "STARE AT THE BLANK SCREEN UNTIL IT FLICKERS.",

    // === [ 呼吸与运动：感知肉体的极限 ] ===
    "RUN UNTIL YOUR LUNGS BURN, THEN KEEP WALKING.",
    "LACE YOUR SHOES TIGHTLY AND SPRINT FOR EXACTLY 400 METERS IN THE DEAD OF NIGHT. DO NOT LOOK BACK, EVEN IF YOU HEAR FOOTSTEPS MATCHING YOUR PACE.",
    "HOLD YOUR BREATH AS IF SINKING IN THE LAKE.",
    "COUNT YOUR HEARTBEATS FOR TWO MINUTES.",

    // === [ 垂钓与深渊：等待未知的咬钩 ] ===
    "CAST YOUR LINE WHERE THERE IS NO WATER.",
    "TIE A RUSTY HOOK TO AN INVISIBLE THREAD. WAIT BY THE EDGE OF A MURKY PUDDLE UNTIL YOU FEEL A TUG. DO NOT REEL IT IN. JUST LET GO.",
    "WAIT FOR A BITE ON AN EMPTY HOOK.",

    // === [ 循环与因果：无言的修行 ] ===
    "LIGHT THREE STICKS OF INCENSE IN THE DARK.",
    "WALK CLOCKWISE AROUND THE NEAREST TREE 108 TIMES. WITH EACH STEP, FORGET ONE WORD YOU KNOW. WHEN YOU FINISH, REMAIN SILENT FOR THE REST OF THE DAY.",
    "FLIP THROUGH THE PAGES OF A BOOK YOU HAVE NEVER READ. STOP AT PAGE 199, READ THE FIRST SENTENCE OUT LOUD, AND THEN TEAR THE PAGE OUT.",
    
    // === [ 纯正都市风：诡异的社交交互 ] ===
    "POINT AT THE CEILING AND WHISPER 'YES'.",
    "FIND A STRANGER WEARING A BLUE COAT. HAND THEM A FOLDED PIECE OF PAPER WITH THE WORD 'TOMORROW' WRITTEN ON IT, AND IMMEDIATELY WALK AWAY WITHOUT BLINKING."
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