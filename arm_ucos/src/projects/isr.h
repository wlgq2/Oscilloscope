#ifndef __ISR_H
#define __ISR_H 

#include  "common_header.h"
#include "task_header.h"

#if     USOC_EN > 0u    //如果定义了用ucos系统  用ucos的滴答定时器中断服务函数
#undef  VECTOR_014
#undef  VECTOR_015
#define VECTOR_014    OS_CPU_PendSVHandler
#define VECTOR_015    OS_CPU_SysTickHandler
#endif  //if     USOC_EN > 0u  


#undef  VECTOR_106                        //取消中断号的定义
#define VECTOR_106    PORTD_IRQHandler    //重新定义硬件上访中断服务函数

#undef  VECTOR_084                        //取消中断号的定义
#define VECTOR_084    PIT0_IRQHandler    //重新定义硬件上访中断服务函数

void PORTD_IRQHandler(void);           //PORTD中断服务函数
void PIT0_IRQHandler(void);
#endif  