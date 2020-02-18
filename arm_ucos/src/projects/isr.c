#include "isr.h"
#include "bsp.h"


//按键中断
void PORTD_IRQHandler(void)
{
  
    if(PORTD_ISFR & (1<<8))             //PTd8触发中断
    {
        OSFlagPost(KeyFlag,(OS_FLAGS)0x01,OS_FLAG_SET,NULL);
        PORTD_ISFR  |= (1<<8);          //写1清中断标志位 
      
    }     
    else if(PORTD_ISFR & (1<<10))             
    {
        OSFlagPost(KeyFlag,(OS_FLAGS)0x02,OS_FLAG_SET,NULL);
        PORTD_ISFR  |= (1<<10);        
   
    }    
    else if(PORTD_ISFR & (1<<12))           
    {
        OSFlagPost(KeyFlag,(OS_FLAGS)0x04,OS_FLAG_SET,NULL);
        PORTD_ISFR  |= (1<<12);         
    
    }    
    else if(PORTD_ISFR & (1<<14))          
    {
        OSFlagPost(KeyFlag,(OS_FLAGS)0x08,OS_FLAG_SET,NULL);
        PORTD_ISFR  |= (1<<14);         
    
    }    
    else;
}

void PIT0_IRQHandler(void)
{
    OSFlagPost(Data_Process,(OS_FLAGS)0x01,OS_FLAG_SET,NULL);
    
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;   //清中断标志
    PIT_MCR = 0;     
}

