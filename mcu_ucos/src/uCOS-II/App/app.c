#include <app.h>
#include  "ucos_ii.h"  		//uC/OS-II系统函数头文件
#include  "sysinit.h"          //系统配置


#include "task_header.h" //任务相关宏 结构体
#include "fpga_data.h"   //与FPGA通信部分
#include "bsp.h"       //ARM驱动


#include "character.h"   //字模
#include "dds_data.h"   //dds数据

#include "mcu_math.h"    //数学算法部分
//#include "arm_math.h" 
/********临时定义变量******************/
uint8 led_2 =0;
uint8 cc[4];
uint16 debug,debug1,debug2,debug3;


void system_init_task(void *pdata)
{
    
    //double cc;

    pdata = pdata;
    
#if OS_CRITICAL_METHOD == 3   //关中断的方法为3
    OS_CPU_SR cpu_sr;
#endif
    
    OS_ENTER_CRITICAL();
    OS_CPU_SysTickInit(CPU_frequency/1000); //初始化OS节拍无比重要
    
    port_init();          //设置所有IO为低电平&&输出
    
    //UART4_Init(115200);
    init_LED();
    key_init();         //将四个IO配备成中断
    lcd_data_init();     //lcd 数据传输
    PIT0_Init(CPU_frequency/20);     //pit定时中断，VGA刷屏速率约为63次/秒
    //PIT0_Init(5000000); 
    
    //初始化四个电位器AD
    AD_init();
    
    OS_EXIT_CRITICAL();
    //计算sin相关参数
    
    
    read_data_init();
    
    LED1(0);
    LED2(0);
    LED3(0);
    LED4(0);
    
    //状态
    trigger.state =0;
    kalman.state = 0;
    fft_wave.state =0;
    freq_selc.state =0;
    dds.state = 0;
    trigger_lock.state =0;
    
    //默认值
    trigger.value =0;
    kalman.value = 0;
    fft_wave.value =0;
    freq_selc.value =0;
    dds.value = 0;
    trigger_lock.value =0;
    
    //光标
    trigger.cursor =0;
    kalman.cursor = 0;
    fft_wave.cursor =0;
    freq_selc.cursor =0;
    dds.cursor = 0;
    trigger_lock.cursor =0;
    
    //双向循环链表 我勒个擦
    fft_wave.next_menu = &trigger;
    trigger.last_menu = &fft_wave;
    fft_wave.value_num = 2;
    fft_wave.menu_num =1;
    
    trigger.next_menu =  &kalman;
    kalman.last_menu = &trigger;
    trigger.value_num = 3;//上升 下降 双向
    trigger.menu_num =2;
    
    kalman.next_menu = &freq_selc;
    freq_selc.last_menu = &kalman;
    kalman.value_num =2;
    kalman.menu_num =3;
    
    freq_selc.next_menu = &trigger_lock;
    trigger_lock.last_menu = &freq_selc;
    freq_selc.value_num = 3;//开启，关闭，确认输入
    freq_selc.menu_num =4;
    
    trigger_lock.next_menu = &dds;
    dds.last_menu = &trigger_lock;
    trigger_lock.value_num = 2;//锁定，解锁
    trigger_lock.menu_num =5;
    
    dds.next_menu = &fft_wave;
    fft_wave.last_menu = &dds;
    dds.value_num = 4;//方波，正弦，合成波（滤波演示），干扰波（卡尔曼演示）
    dds.menu_num =6;
    
    
    menu_p = &fft_wave;
    menu_head = &fft_wave;
  
    
    //key事件标志
    KeyFlag = OSFlagCreate(0,&err);
    
    Menu = OSFlagCreate(0,&err);
    
    Data_Process = OSFlagCreate(0,&err);
    //menu信号量，保护全局变量 无比重要
    MenuSem = OSSemCreate(1);
    FirParamSem = OSSemCreate(1);
    //这里换成互斥信号量，避免优先级反转标记一下。
    //LCDdataSem = OSSemCreate(1);
    LCDdataSem = OSMutexCreate(Mutex_prio,&err);
    
    //发送参数的邮箱
    ParaMbox = OSMboxCreate((void*)0);
    FirMbox = OSMboxCreate((void*)0);    
    
    OSFlagPost(Menu,(OS_FLAGS)0x01,OS_FLAG_SET,NULL);
   
    
    /*临时测试add与clk信号是否能用*/
    OSTimeDly(5);
    OSTaskDel(Init_task_prio);
}
void key_task(void *pdata)
{
    pdata = pdata;
    
    while(1)
    {
                                            //等待置1           
        OSFlagPend(KeyFlag,(OS_FLAGS)0x0f,OS_FLAG_WAIT_SET_OR,0,&err);
        OSTimeDly(40);
        if(!((GPIOD_PDIR>>8)&0x01))    //确认键
        {
            led_2 = !led_2;
            while(!((GPIOD_PDIR>>8)&0x01))
                OSTimeDly(1);
            OSTimeDly(40);
            LED2(led_2);
            key_menu(1);
            OSFlagPost(KeyFlag,(OS_FLAGS)0x01,OS_FLAG_CLR,NULL);
            OSFlagPost(Menu,(OS_FLAGS)0x01,OS_FLAG_SET,NULL);
            OSFlagPost(FirFlag,(OS_FLAGS)0x01,OS_FLAG_SET,NULL);
        }
        else if(!((GPIOD_PDIR>>10)&0x01))  //退出键
        {
            led_2 = !led_2;
            while(!((GPIOD_PDIR>>10)&0x01))
                OSTimeDly(1);
            OSTimeDly(40);
            LED2(led_2);
            key_menu(2);
            OSFlagPost(KeyFlag,(OS_FLAGS)0x02,OS_FLAG_CLR,NULL);
            OSFlagPost(Menu,(OS_FLAGS)0x02,OS_FLAG_SET,NULL);
        }
        else if(!((GPIOD_PDIR>>12)&0x01))    //左
        {
            led_2 = !led_2;
            while(!((GPIOD_PDIR>>12)&0x01))
                OSTimeDly(1);
            OSTimeDly(20);
            LED2(led_2);
            key_menu(3);
            OSFlagPost(KeyFlag,(OS_FLAGS)0x04,OS_FLAG_CLR,NULL);
            OSFlagPost(Menu,(OS_FLAGS)0x04,OS_FLAG_SET,NULL);
        }
        else if(!((GPIOD_PDIR>>14)&0x01))   //右
        {
            led_2 = !led_2;
            while(!((GPIOD_PDIR>>14)&0x01))
                OSTimeDly(1);
            OSTimeDly(20);
            LED2(led_2);
            key_menu(4);
            OSFlagPost(KeyFlag,(OS_FLAGS)0x08,OS_FLAG_CLR,NULL);
            OSFlagPost(Menu,(OS_FLAGS)0x08,OS_FLAG_SET,NULL);
        }
        else
            OSFlagPost(KeyFlag,(OS_FLAGS)0x0f,OS_FLAG_CLR,NULL);//清按键数据
        
    }
    
    
}

void uart4_task(void *pdata)
{
    pdata = pdata;
    
    
    while(1)
    {
       /* LED1(0);
        Uart4_SendByte(97);
    
        OSTimeDly(100);
        LED1(1);
        OSTimeDly(100);*/
    }
}


void menu_task(void * pdata)
{

    //uint8 data;
    
#if OS_CRITICAL_METHOD == 3   //关中断的方法为3
    OS_CPU_SR cpu_sr;
#endif
    
	pdata = pdata;
        //cpu_sr = cpu_sr;

    while(1)
    {
        OSFlagPend(Menu,(OS_FLAGS)0x0f,OS_FLAG_WAIT_SET_OR+OS_FLAG_CONSUME,0,&err); //所有按键皆触发该事件

        //OSSemPend(LCDdataSem,0,&err);
        OSMutexPend(LCDdataSem,0,&err);
        show_menu();
        
        //OSSemPost(LCDdataSem);
        OSMutexPost(LCDdataSem);
        OSFlagPost(Menu,(OS_FLAGS)0x01,OS_FLAG_CLR,NULL);
        
    }
}



void data_process_task(void * pdata)
{
#if OS_CRITICAL_METHOD == 3   //关中断的方法为3
    OS_CPU_SR cpu_sr;
#endif
    
    uint16 i,j,trigger_add,add;
    uint8 data;
    int8 tem;
    uint8 wave_data[(LcdLength+FirLength-1)];
    uint16 ReadLength;
    
    struct complex_num fft_result[FFT_CNT];
    uint8 fft_out[FFT_CNT];
    
    int32 FirTem;
    //uint8 wocao[4] = {0x01,0x04,0x02,0x03};

    //uint8 sample_para,voltage_para,phase_para,show_para;
    uint8 Para[4]; //sample show phase vlotage
    //uint8 show_num,;
    
    pdata = pdata;

    while(1)
    {   
        OSFlagPend(Data_Process,(OS_FLAGS)0x01,OS_FLAG_WAIT_SET_AND+OS_FLAG_CONSUME,0,&err);
        //cc = arm_sin_f32(1.6);
        if(!trigger_lock.value)  //如果触发
        {
            //获取AD信号，分频
            Para[0] = ADC_Getdata(sample_ctrl);
            Para[0] = sample_algorithms(Para[0]);
            debug2 = Para[0];
            
            Set_fre_div(Para[0]);//1为2分频，2为四分频……
            //开始采集
            read_start(1);
            
            //寻找触发源
            trigger_add = trigger_seek();
        }
        else
        {
            
        }

        Para[1] = (ADC_Getdata(show_ctrl)>>4)+1;
        debug = Para[1];
        
        Para[2] = ADC_Getdata(phase_ctrl);
        Para[2] =(Para[2]>>2)<<2;
        
        debug1 = Para[2];
        
        
        OSMboxPost(ParaMbox,&Para[0]);
        //phase_para = 0;
        //debug = phase_para;
		//发送消息
        add = trigger_add+Para[2]*Para[1];
        
        //读取更多数据以完成FIR滤波
        if(freq_selc.value)
            ReadLength = (LcdLength+FirLength-1);
        else
            ReadLength = LcdLength;
        
        
        for(i=0;i<ReadLength;i++)
        {
            //OS_ENTER_CRITICAL();
            add += Para[1];
            wave_data[i] = read_data(add);
            //OS_EXIT_CRITICAL();
        }
        read_start(0);

        if(freq_selc.value==2)
        {
            
            for(i=0;i<LcdLength;i++)
            {
                FirTem=0;
                for(j=0;j<FirLength;j++)
                    FirTem += wave_data[i+j]*FirParam[j];
                wave_data[i]=FirTem>>10;
            }
                    
        }
        else if(freq_selc.value==1)
        {
            
            for(i=0;i<LcdLength;i++)
            {
                FirTem=0;
                for(j=0;j<FirLength;j++)
                    FirTem += wave_data[i+j]*FirParam[j];
                wave_data[i]=(FirTem>>10)-100;
            }
                    
        }
        else;          

        if(kalman.value)
            KalmanFilter(&wave_data[0],KalmanInitX,KalmanInitP);
        else;

        if(fft_wave.value==1)
        {
            OS_ENTER_CRITICAL();
            fft(&wave_data[0],&fft_result[0],FFT_LEN,FFT_CNT);
            absfft(&fft_result[0],&fft_out[0],FFT_CNT);
            OS_EXIT_CRITICAL();
            //cc[0] = fft_out[0];
            //cc[1] = fft_out[1];
            //cc[2] = fft_out[2];
            //cc[3] = fft_out[3];
            OSMutexPend(LCDdataSem,0,&err);
            //命令切换
            data = (menu_p->state)?(0x80+clr_data_cnt):clr_data_cnt;
            write_command(data);
        
            //写波形数据
            data = (menu_p->state)?(0x80+write_data_command):write_data_command;
            write_command(data);
            
            for(i=0;i<(240);i++)
            {
				tem = (fft_out[i+1]-fft_out[i])/2;
                write_data(fft_out[i]);
				write_data(fft_out[i]+tem);
            }
        
            OSMutexPost(LCDdataSem);  
        }
        else
        {
            OSMutexPend(LCDdataSem,0,&err);
            //命令切换
            data = (menu_p->state)?(0x80+clr_data_cnt):clr_data_cnt;
            write_command(data);
        
            //写波形数据
            data = (menu_p->state)?(0x80+write_data_command):write_data_command;
            write_command(data);
            
            for(i=0;i<480;i++)
            {
                write_data(wave_data[i]);
            }
            OSMutexPost(LCDdataSem);  
        }
        OSFlagPost(Data_Process,(OS_FLAGS)0x01,OS_FLAG_CLR,NULL);
        
    }
    
}

void dds_ctrl_task(void *pdata)
{
    pdata = pdata;
    while(1)
    {
        
        OSFlagPend(Menu,(OS_FLAGS)0x01,OS_FLAG_WAIT_SET_AND+OS_FLAG_CONSUME,0,&err);  //确认键触发该事件
        //OSFlagPend(Menu,(OS_FLAGS)0x01,OS_FLAG_WAIT_SET_AND,0,&err);  //尝试清除/不清除标志
        if((menu_p ==&dds)&&(!menu_p->state)) //在DDS选项下按了确认 并且状态非下拉
        {
            OSMutexPend(LCDdataSem,0,&err);
            dds_data(dds.value);
            OSMutexPost(LCDdataSem); 
        }
        else
        {
            //OSFlagPost(Menu,(OS_FLAGS)0x01,OS_FLAG_CLR,NULL);
            //标志不清除，留给menu事件用。
        }
          
    }
}


void FirPara_task(void *pdata)
{
    pdata = pdata;
    uint8 * ReceivePara;
    uint32 FirFre;
    while(1)
    {
        
        OSFlagPend(Menu,(OS_FLAGS)0x01,OS_FLAG_WAIT_SET_AND+OS_FLAG_CONSUME,0,&err);  //确认键触发该事件
        
        if((menu_p ==&freq_selc)&&(!menu_p->state))
        //计算fir参数
        {
            ReceivePara = OSMboxPend(ParaMbox,0,&err);
            if(freq_selc.value==1)
            {
                FirFre = ReceivePara[1]*3*(ReceivePara[0]-1)/25;
                FirPara(255,FirFre,DataLength,&FirParam[0],FirLength);
            }
            else if(freq_selc.value==2)
            {
                FirFre = ReceivePara[1]*12*(ReceivePara[0]-1)/25;
                FirPara(FirFre,0,DataLength,&FirParam[0],FirLength);
            }
            else;
        }
         OSTimeDly(10);
       
    }
    //参数计算 参数 采样频率 分频 DDS频率
    //FirPara(uint8 H,uint8 L, uint16 N,float * fir_c,FirLength)
}


void Paradata_task(void *pdata)
{
    uint8 * ReceivePara;
    pdata =pdata;
    
    while(1)
    {
	ReceivePara = OSMboxPend(ParaMbox,0,&err);
        OSMutexPend(LCDdataSem,0,&err);
        show_para(ReceivePara);
        OSMutexPost(LCDdataSem);
        OSTimeDly(3);
    }
}