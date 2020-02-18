#include "task_header.h"


uint8 err;

struct menu trigger,kalman,fft_wave,freq_selc,trigger_lock,dds;
struct menu * menu_p,* menu_head;


OS_FLAG_GRP *KeyFlag;

OS_FLAG_GRP *Menu;
OS_FLAG_GRP *FirFlag;
OS_FLAG_GRP *Data_Process;

OS_EVENT *MenuSem;
OS_EVENT *FirParamSem;
OS_EVENT *LCDdataSem;
OS_EVENT *ParaMbox;
OS_EVENT *FirMbox;

uint8 LCD_Command;

int32 FirParam[FirLength];

void key_menu(uint8 key_num)
{
    OSSemPend(MenuSem,0,&err);
    
    switch(key_num)
    {
        case 1:        //确认键
            if(menu_p->state)
            {
                menu_p->state =0; //不展开
                menu_p->value = menu_p->cursor;
                //menu_p->cursor =0;
            }
            else
            {
                menu_p->state =1; //展开
                menu_p->cursor =0;
            }
            break;
        case 2:         //退出键
            menu_p->state =0; //不展开
            break;
            
        case 3:          //左
            if(menu_p->state)
            {
                if(menu_p->cursor)
                    menu_p->cursor = (menu_p->cursor-1);
                else
                    menu_p->cursor = (menu_p->value_num-1);
            }
            else
            {
                menu_p = menu_p->last_menu;
            }
            break;
        case 4:         //右
            if(menu_p->state)
            {
                if(menu_p->cursor ==(menu_p->value_num-1))
                    menu_p->cursor = 0;
                else
                    menu_p->cursor = (menu_p->cursor+1);
            }
            else
            {
                menu_p = menu_p->next_menu;
            }
            break;
        default:
            break;
    }
    
    OSSemPost(MenuSem);
    
}

uint8 sample_algorithms(uint8 AD_data)
{
    if(AD_data<10)
        AD_data =0;
    else if(AD_data<18)
        AD_data = 1;
    else if(AD_data<25)
        AD_data =2;
    else if(AD_data<30)
        AD_data = 3;
    else if(AD_data<34)
        AD_data = 4;
    else if(AD_data<37)
        AD_data = 5;
    else if(AD_data<40)
        AD_data = 6;
    else if(AD_data>200)
        AD_data -= 113;
    else
        AD_data = (AD_data-26)>>1;
}
