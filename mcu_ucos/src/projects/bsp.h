#ifndef BSP_H
#define  BSP_H

#include "common_header.h"

#define  SYS_CLK                        (100000000L)          // 96MHz
#define  PERIPH_SYS_CLK                 (U32)(SYS_CLK/(((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1))

#define sample_ctrl        0x01
#define voltage_ctrl       0x02
#define phase_ctrl        0x03
#define show_ctrl          0x04

#define LED1(a) if(a)\
                   GPIOA_PDOR &= ~(1<<4);\
               else\
                   GPIOA_PDOR |= (1<<4)   

#define LED2(a) if(a)\
                   GPIOA_PDOR &= ~(1<<6);\
               else\
                   GPIOA_PDOR |= (1<<6)    

#define LED3(a) if(a)\
                   GPIOA_PDOR &= ~(1<<8);\
               else\
                   GPIOA_PDOR |= (1<<8) 

#define LED4(a) if(a)\
                   GPIOA_PDOR &= ~(1<<10);\
               else\
                   GPIOA_PDOR |= (1<<10)    

                     
#define lcd_wr(a) if(a)\
                      GPIOB_PDOR |= (1<<6);\
                  else\
                      GPIOB_PDOR &= ~(1<<6)
                                         
                     
#define lcd_oe(a) if(a)\
                      GPIOB_PDOR |= (1<<8);\
                  else\
                      GPIOB_PDOR &= ~(1<<8)
                        
                        
#define read_start(a) if(a)\
                      GPIOB_PDOR |= (1<<3);\
                  else\
                      GPIOB_PDOR &= ~(1<<3)
                        
#define Fre_wirte(a) if(a)\
                      GPIOA_PDOR |= (1<<8);\
                  else\
                      GPIOA_PDOR &= ~(1<<8)                    
                        
void init_LED(void);
void Delay(uint32 time);
void Delay_cnt(uint32 time);
void port_init(void);
void key_init(void);
void PIT0_Init(uint32 pit_cnt);

void UART4_Init(uint32);
void Uart4_SendByte(uint8);
uint8 Uart4_Receive(void);

void lcd_data_init(void);
void lcd_data_write(uint8 lcd_data);

void read_data_init(void);
void set_add(uint16 add);
uint8 read_data(uint16 add);
void AD_init(void);
#endif
