#ifndef task_header_h

#define task_header_h

#include "common_header.h"

//定义与fpga交互命令
#define clr_menu_cnt         0x00 //清除显示RAM计数
#define write_menu_ram       0x02 //菜单RAM区
#define write_dds_ram        0x03
#define write_data_command   0x01
#define clr_data_cnt         0x04 //清除波形数据计数
#define clr_dds_add          0x05
#define clr_para_add         0x06
#define para_ram             0x07

#define FFT_CNT              256
#define FFT_LEN              8

#define Mutex_prio             12 //互斥信号量 主要是防止优先级反转

#define data_process_prio       13  
#define data_process_size       8192 //分配堆栈

//key_task为响应按键中断，优先级还是高点比较好（否则容易被数据处理部分打断得不到响应）
#define Key_task_prio          11
#define Key_task_size          2000 //分配堆栈


#define Init_task_prio         10
#define Init_task_size         64 

//调试用
#define uart4_task_prio         25
#define uart4_task_size         64 

//菜单任务的实时性要求低，数据量大。优先级要最低。
#define menu_task_prio         20
#define menu_task_size         1024

#define ddsCtrl_task_prio         19
#define ddsCtrl_task_size         1024

#define FirPara_task_prio         15
#define FirPara_task_size         500

#define Paradata_task_prio         16
#define Paradata_task_size         500

#define    Trigger_Num             2
#define    Trigger_Limin           1
#define    TriggerPoint            0xb0

#define    FirLength             21
#define    MidFre                8
#define    DataLength             512

#define    LcdLength              480


struct menu
{
    uint8 state;    //0：不显示 1:展开   0：不展开
    uint8 value;   //设置的值
    uint8 cursor;  //下拉光标更随的值
    uint8 value_num; //值的数量
    uint8 menu_num; //菜单位置
    
    struct menu * next_menu; 
    struct menu * last_menu;
};

extern uint8 err;

extern struct menu trigger,kalman,fft_wave,freq_selc,trigger_lock,dds;
extern struct menu * menu_p;
extern struct menu * menu_head;

extern OS_FLAG_GRP *KeyFlag;

extern OS_FLAG_GRP *Menu;

extern OS_FLAG_GRP *Data_Process;
extern OS_FLAG_GRP *FirFlag;


extern OS_EVENT *MenuSem;
extern OS_EVENT *LCDdataSem;
extern OS_EVENT *ParaMbox;
extern OS_EVENT *FirParamSem;
extern OS_EVENT *FirMbox;

extern uint8 LCD_Command;

extern int32 FirParam[FirLength];

void key_menu(uint8 key_num);
void show_menu(void);

uint8 sample_algorithms(uint8 AD_data);

#endif

