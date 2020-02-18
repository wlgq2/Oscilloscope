#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include "common.h"




/***************** ucos 专用 *****************/
#define USOC_EN 1u
#include  "ucos_ii.h"               //uC/OS-II系统函数头文件

#include  "app.h"           //用户任务函数


#define EnableInterrupts asm(" CPSIE i");  //开总中断

#define DisableInterrupts asm(" CPSID i");  //关总中断

#define CPU_frequency   100000000


//typedef unsigned short    uint16;
//typedef unsigned long     uint32;
//typedef unsigned char     uint8;

#define      uint16            unsigned short
#define      uint32            unsigned long
#define      uint8             unsigned char 

#define    ARM_MATH_CM4           4 

#endif
