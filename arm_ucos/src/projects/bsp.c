#include "bsp.h"

void port_init(void)
{

    GPIOA_PDOR = 0x00;
    GPIOB_PDOR = 0x00;
    GPIOC_PDOR = 0x00;
    GPIOD_PDOR = 0x00;
    GPIOE_PDOR = 0x00;
    
    GPIOA_PDDR = 0xffffffff;      //设置为输出
    GPIOB_PDDR = 0xffffffff;   
    GPIOC_PDDR = 0xffffffff;   
    GPIOD_PDDR = 0xffffffff;   
    GPIOE_PDDR = 0xffffffff;
  
}

void key_init(void)
{
    SIM_SCGC5 |= (1<<12);      //开启PORTD端口
  
    PORTD_PCR8  |= (1<<8) | (10<<16) | (3);  //GPIO 下降沿触发 上拉电阻
    PORTD_PCR10 |= (1<<8) | (10<<16) | (3);  
    PORTD_PCR12 |= (1<<8) | (10<<16) | (3);  
    PORTD_PCR14 |= (1<<8) | (10<<16) | (3);  
    
    GPIOD_PDDR &= ~((1<<8)|(1<<10)|(1<<12)|(1<<14));           //输入模式
    enable_irq(90);   //使能PORT中断
}

void init_LED(void)
{

    PORTA_PCR4 &= ~0x700;       //MUX为000
    PORTA_PCR4 |= PORT_PCR_MUX(1);      //MUX 001 GPIO模式
    PORTA_PCR6 &= ~0x700;
    PORTA_PCR6 |= PORT_PCR_MUX(1);  
    PORTA_PCR8 &= ~0x700;
    PORTA_PCR8 |= PORT_PCR_MUX(1);  
    PORTA_PCR10 &= ~0x700;
    PORTA_PCR10 |= PORT_PCR_MUX(1);  
  
    GPIOA_PDDR |= ((1<<10)|(1<<8)|(1<<4)|(1<<6));      //设置为输出
}

void Delay(uint32 time)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    LPTMR0_CMR = time;
    LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;  //PCS:01 LPO——1KHz的时钟 PBYP:1
    LPTMR0_CSR = LPTMR_CSR_TEN_MASK; //使能定时器
    while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)); //延时 溢出复位
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK; //清零
}

void Delay_cnt(uint32 time)
{
    while(time--);
}

void PIT0_Init(uint32 pit_cnt)
{
 
    SIM_SCGC6 |= (1<<23);
    
    PIT_MCR = 0;
     
    PIT_LDVAL0 = (pit_cnt);
    
    PIT_TCTRL0 |= (3);  
    
    enable_irq(68);
    
}



void UART4_Init(uint32 BaudRate)
{
  uint16 ubd, brfa;

  uint8 temp;
   /*使用C14，C15*/
  //PORTC_PCR15 = PORT_PCR_MUX(0x3);   //引脚使能 
  //PORTC_PCR14 = PORT_PCR_MUX(0x3); 
  
     /*使用E24，C15*/
  PORTE_PCR25 = PORT_PCR_MUX(0x3);   //引脚使能 
  PORTE_PCR24 = PORT_PCR_MUX(0x3); 
  
  SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;   //时钟使能


  UART4_C2 &= ~((1<<2) | (1<<3)); //禁止发送接收   控制寄存器C2
  
  UART4_C1 = 0;  //控制器C1 全部使用默认配备

  ubd = (uint16)((CPU_frequency>>1)/(BaudRate * 16));   //波特率设置

  UART4_BDH = (uint8)ubd>>8;  
  UART4_BDL = (uint8)(ubd);
  
  
  brfa = ((((CPU_frequency>>1)*32)/(BaudRate * 16)) - (ubd * 32));  //波特率微调 1/32的增量

  temp = UART4_C4 & ~(UART_C4_BRFA(0x1F)); 
  UART4_C4 = temp | UART_C4_BRFA(brfa);
  

  UART4_C2 |= ((1<<2) | (1<<3));  //允许发送接收

  return;
}

void Uart4_SendByte(uint8 Char)
{

    while(!(UART_S1_REG(UART4_BASE_PTR) & UART_S1_TDRE_MASK)); //UARTx_S1状态寄存器
    UART4_D = Char;
    return;
}

uint8 Uart4_Receive(void)
{
    uint8 Receivechar;
    while(!(UART_S1_REG(UART4_BASE_PTR) & UART_S1_RDRF_MASK));       //查询是否接受到数据
    Receivechar  =   UART4_D;                    //接受到8位的数据
    return(Receivechar);
}

void lcd_data_init(void)
{
    SIM_SCGC5 |= (1<<10);      //开启PORTB端口
    SIM_SCGC5 |= (1<<11);      
    
    
    //port设置
    PORTB_PCR6  = (1<<8);   //GPIO模式，关中断，关滤波，关强上拉，关开漏
    PORTB_PCR8  = (1<<8);
    PORTB_PCR10 = (1<<8);
    PORTB_PCR16 = (1<<8);   
    PORTB_PCR18 = (1<<8);
    PORTB_PCR20 = (1<<8);   
    PORTB_PCR22 = (1<<8);     
    
    PORTC_PCR0 = (1<<8);
    PORTC_PCR2 = (1<<8);   
    PORTC_PCR4 = (1<<8);
       
    
    GPIOB_PDDR |= ((1<<6)|(1<<8)|(1<<10)|(1<<16)|(1<<18)|(1<<20)|(1<<22));      //设置为输出
    GPIOC_PDDR |= ((1)|(1<<2)|(1<<4));    
    
    GPIOB_PDOR &= ~((1<<6)|(1<<8)|(1<<10)|(1<<16)|(1<<18)|(1<<20)|(1<<22));    //清零
    GPIOC_PDOR &= ~((1)|(1<<2)|(1<<4));
}


void lcd_data_write(uint8 lcd_data)
{
    if(lcd_data&0x01)
        GPIOB_PDOR |= (0x400);
    else
        GPIOB_PDOR &= ~(1<<10);
                        
    if(lcd_data&0x02)
        GPIOB_PDOR |= 0x10000;
    else
        GPIOB_PDOR &= ~(1<<16);

    if(lcd_data&0x04)
        GPIOB_PDOR |= 0x40000;
    else
        GPIOB_PDOR &= ~(1<<18);

    if(lcd_data&0x08)
        GPIOB_PDOR |= 0x100000;
    else
        GPIOB_PDOR &= ~(1<<20);

    if(lcd_data&0x10)
        GPIOB_PDOR |= 0x400000;
    else
        GPIOB_PDOR &= ~(1<<22);

    if(lcd_data&0x20)
        GPIOC_PDOR |= 0x01;
    else
        GPIOC_PDOR &= ~(1);
    
    if(lcd_data&0x40)
        GPIOC_PDOR |= 0x04;
    else
        GPIOC_PDOR &= ~(1<<2);

    if(lcd_data>>7)
        GPIOC_PDOR |= 0x10;
    else
        GPIOC_PDOR &= ~(1<<4);
}

void read_data_init(void)
{
    SIM_SCGC5 |= (1<<9);      //开启PORT端口
    SIM_SCGC5 |= (1<<10);
    SIM_SCGC5 |= (1<<11);   
    //SIM_SCGC5 |= (1<<12); 
    SIM_SCGC5 |= (1<<13);   
    
    /*
    端口
    8位数据线 13位地址线 8kb寻址空间
    1位分频选择信号线。1位clk线
    其中地址线，兼做分频参数选择
    数据线:A13 A15 A17 A19 A25 A27 A29 B1 start信号:B3;
    地址线:C6 C8 C10 C12 C14 C16 D0 C19
    E6 E8 E10 E24 E26  分频A8
    */
    //data wire
    
    
    //E28无法使用，测试发现其和地短接
    //暂用接LED的A8代替，A8有点问题，低电平为0.1v
    PORTA_PCR13 = (1<<8);   //GPIO模式，关中断，关滤波，关强上拉，关开漏
    PORTA_PCR15 = (1<<8);
    PORTA_PCR17 = (1<<8);
    PORTA_PCR19 = (1<<8);   
    PORTA_PCR25 = (1<<8);
    PORTA_PCR27 = (1<<8);   
    PORTA_PCR29 = (1<<8);  
    PORTB_PCR1  = (1<<8);
    
    //clk
    PORTB_PCR3  = (1<<8);
    //add
    PORTC_PCR6  = (1<<8);
    PORTC_PCR8  = (1<<8);   
    PORTC_PCR10 = (1<<8);   
    PORTC_PCR12 = (1<<8);
    PORTC_PCR14 = (1<<8);   
    PORTC_PCR16 = (1<<8); 
    PORTC_PCR17 = (1<<8); 
    PORTC_PCR18 = (1<<8); 
    
    PORTE_PCR6  = (1<<8);
    PORTE_PCR8  = (1<<8);   
    PORTE_PCR10 = (1<<8);   
    PORTE_PCR26 = (1<<8);  
    PORTE_PCR24 = (1<<8);
    
    PORTA_PCR8  = (1<<8);
    
    
    GPIOC_PDDR |= ((1<<6)|(1<<8)|(1<<10)|(1<<12)|(1<<14)|(1<<16)|(1<<17)|(1<<18));      //设置为输出     
    GPIOB_PDDR |= (1<<3);
    GPIOE_PDDR |= ((1<<6)|(1<<8)|(1<<10)|(1<<26)|(1<<28));   
    GPIOA_PDDR |= (1<<8);
    //设置为输入
    GPIOA_PDDR &= ~((1<<13)|(1<<15)|(1<<17)|(1<<19)|(1<<25)|(1<<27)|(1<<29));
    GPIOB_PDDR &= ~(1<<1);
}

void set_add(uint16 add)
{
    if(add&0x01)
        GPIOC_PDOR |= (1<<6);
    else
        GPIOC_PDOR &= ~(1<<6);
                        
    if(add&0x02)
        GPIOC_PDOR |= (1<<8);
    else
        GPIOC_PDOR &= ~(1<<8);

    if(add&0x04)
        GPIOC_PDOR |= (1<<10);
    else
        GPIOC_PDOR &= ~(1<<10);

    if(add&0x08)
        GPIOC_PDOR |= (1<<12);
    else
        GPIOC_PDOR &= ~(1<<12);

    if(add&0x10)
        GPIOC_PDOR |= (1<<14);
    else
        GPIOC_PDOR &= ~(1<<14);

    if(add&0x20)
        GPIOC_PDOR |= (1<<16);
    else
        GPIOC_PDOR &= ~(1<<16);
    
    if(add&0x40)
        GPIOE_PDOR |= (1<<6);
    else
        GPIOE_PDOR &= ~(1<<6);
    
    if(add&0x80)
        GPIOE_PDOR |= (1<<8);
    else
        GPIOE_PDOR &= ~(1<<8);

    if(add&0x100)
        GPIOE_PDOR |= (1<<10);
    else
        GPIOE_PDOR &= ~(1<<10);
    
    if(add&0x200)
        GPIOC_PDOR |= (1<<17);
    else
        GPIOC_PDOR &= ~(1<<17);
    
    if(add&0x400)
        GPIOC_PDOR |= (1<<18);
    else
        GPIOC_PDOR &= ~(1<<18);
    
    if(add&0x800)
        GPIOE_PDOR |= (1<<24);
    else
        GPIOE_PDOR &= ~(1<<24);
   
    if(add&0x1000)
        GPIOE_PDOR |= (1<<26);
    else
        GPIOE_PDOR &= ~(1<<26);
   
}

uint8 read_data(uint16 add)
{
    uint8 data;
    set_add(add);
    Delay_cnt(3);
    //读数据
    data = 0x00;
    if((GPIOA_PDIR>>13)&0x01)
        data |= 0x01;
    if((GPIOA_PDIR>>15)&0x01)
        data |= 0x02;
    if((GPIOA_PDIR>>17)&0x01)
        data |= 0x04;
    if((GPIOA_PDIR>>19)&0x01)
        data |= 0x08;
    if((GPIOA_PDIR>>25)&0x01)
        data |= 0x10;
    if((GPIOA_PDIR>>27)&0x01)
        data |= 0x20;
    if((GPIOA_PDIR>>29)&0x01)
        data |= 0x40;
    if((GPIOB_PDIR>>1)&0x01)
        data |= 0x80;
    
    
    return(data);
}

void AD_init(void)
{
    SIM_SCGC6 |= (1<<27); //ADC0时钟开启
    SIM_SCGC3 |= (1<<27); //ADC1时钟开启  
    SIM_SOPT7 = 0x00;  
    SIM_SCGC5 |= (1<<12); //开始PORT时钟
    SIM_SCGC5 |= (1<<13);   
    
    PORTE_PCR1 =  PORT_PCR_MUX(0);
    PORTE_PCR3 =  PORT_PCR_MUX(0);
    PORTE_PCR1 =  PORT_PCR_MUX(0);
    PORTE_PCR6 =  PORT_PCR_MUX(0);
    
    ADC0_CFG1 = 0x00000051;
    //不开低功耗，不分频，short time sample模式，8位，bus/2提供时钟

    ADC0_CFG2 = (1<<4);
    //选择AD_b 其他都不开

    ADC0_SC2 &= ~(3);           //选择外部参考电压 3.3v

    ADC0_SC3 |= (1<<2);   // AVGE 1 使能平均

    ADC0_SC3 &= ~(3);           // AVGS 10 4次采样平均
    
    ADC1_CFG1 = 0x00000051;
    //不开低功耗，不分频，short time sample模式，8位，bus提供时钟

    ADC1_CFG2 = 0;
    //选择AD_a 其他都不开

    ADC1_SC2 &= ~(3);           //选择外部参考电压 3.3v

    ADC1_SC3 |= (1<<2);   // AVGE 1 使能平均

    ADC1_SC3 &= ~(3);           // AVGS 10 4次采样平均    
}

uint8 ADC_Getdata(uint8 AD_num)
{

    uint8 ADResult;
    
    switch(AD_num)
    {
        case(sample_ctrl):
            ADC0_SC1A = 0x00000005;  //关中断，开通道，选择通道
            while (!(ADC0_SC1A>>7));   //COCO为转换结束标志
            ADResult = ADC0_RA;
            break;
        case(show_ctrl):
            ADC0_SC1A = 0x00000006; 
            while (!(ADC0_SC1A>>7));  
            ADResult = ADC0_RA;
            break;
        case(phase_ctrl):
            ADC1_SC1A = 0x00000005; 
            while (!(ADC1_SC1A>>7));  
            ADResult = ADC1_RA;
            break;
        case(voltage_ctrl):
            ADC1_SC1A = 0x00000007; 
            while (!(ADC1_SC1A>>7));  
            ADResult = ADC1_RA;
            break;
        default:
            break;
    }

    return(ADResult);
}