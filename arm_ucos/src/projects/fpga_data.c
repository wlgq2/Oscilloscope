#include "fpga_data.h"
#include "dds_data.h"

void write_command(uint8 data)
{
    //OSSemPend(LCDdataSem,0,&err);
    lcd_oe(1);
    lcd_wr(0);
    //lcd_wr(1);
    lcd_data_write(data);
    Delay_cnt(1);
    lcd_wr(1);
    //lcd_wr(0);
    Delay_cnt(2);  
    //OSSemPost(LCDdataSem);

}

void write_data(uint8 data)
{
  
    //OSSemPend(LCDdataSem,0,&err);
    lcd_oe(0);
    lcd_wr(0);
    //lcd_wr(1);
    lcd_data_write(data);
    Delay_cnt(1);
    lcd_wr(1);
    //lcd_wr(0);
    Delay_cnt(2);
    //OSSemPost(LCDdataSem);
}


	/*关于command，
	  0位 1：菜单显示计数清零，0:菜单显示计数不清零
	  1位 1：下拉菜单显示 0：不显示。
	  2~4位置：灌入波形数据(010)，灌入主菜单数据(000)，灌入下拉菜单数据(001),灌入DDS数据(011)，灌入电压频率数据。
          5~7位 ：暂保留（其他区域计数清除）
	  */
void show_menu(void)
{
    uint32 i,j;
    
    LCD_Command = (menu_p->state)?(0x80+clr_menu_cnt):clr_menu_cnt;
    write_command(LCD_Command);//清除cnt
    
    LCD_Command = (menu_p->state)?(0x80+write_menu_ram):write_menu_ram;
    write_command(LCD_Command);
        
    for(i=0;i<16;i++)
    {

        if(menu_p == &fft_wave)
            write_data(char_select[i]);
        else
            write_data(0x00);
            
        write_data(char_xian[(i<<1)]); //显示模式
        write_data(char_xian[(i<<1)|0x01]);       
        write_data(char_shi1[(i<<1)]);
        write_data(char_shi1[(i<<1)|0x01]);    
        write_data(char_mo[(i<<1)]);
        write_data(char_mo[(i<<1)|0x01]);
        write_data(char_shi2[(i<<1)]);
        write_data(char_shi2[(i<<1)|0x01]);
            
        if(menu_p == &trigger)
            write_data(char_select[i]);
        else
            write_data(0x00);
            
        write_data(char_chu[(i<<1)]); //触发设置
        write_data(char_chu[(i<<1)|0x01]);
        write_data(char_fa[(i<<1)]);
        write_data(char_fa[(i<<1)|0x01]);
        write_data(char_she[(i<<1)]);
        write_data(char_she[(i<<1)|0x01]);
        write_data(char_zhi[(i<<1)]);
        write_data(char_zhi[(i<<1)|0x01]);
                    
        if(menu_p == &kalman)
            write_data(char_select[i]);
        else
            write_data(0x00);
                    
        write_data(char_k[i]); //kalman算法
        write_data(char_a[i]);
        write_data(char_l[i]);
        write_data(char_m[i]);
        write_data(char_a[i]);
        write_data(char_n[i]);
        write_data(char_lv2[(i<<1)]);
        write_data(char_lv2[(i<<1)|0x01]);            
        write_data(char_bo[(i<<1)]);
        write_data(char_bo[(i<<1)|0x01]);
                    
        if(menu_p == &freq_selc)
            write_data(char_select[i]);
        else
            write_data(0x00);
                    
        write_data(char_lv2[(i<<1)]); //滤波设置
        write_data(char_lv2[(i<<1)|0x01]);
        write_data(char_bo[(i<<1)]);
        write_data(char_bo[(i<<1)|0x01]);            
        write_data(char_she[(i<<1)]);
        write_data(char_she[(i<<1)|0x01]);
        write_data(char_zhi[(i<<1)]);
        write_data(char_zhi[(i<<1)|0x01]);
                 
        if(menu_p == &trigger_lock)
            write_data(char_select[i]);
        else
            write_data(0x00);
                    
        write_data(char_chu[(i<<1)]); //触发锁定
        write_data(char_chu[(i<<1)|0x01]);
        write_data(char_fa[(i<<1)]);
        write_data(char_fa[(i<<1)|0x01]);
        write_data(char_suo[(i<<1)]);
        write_data(char_suo[(i<<1)|0x01]);
        write_data(char_ding[(i<<1)]);
        write_data(char_ding[(i<<1)|0x01]);
            
        if(menu_p == &dds)
            write_data(char_select[i]);
        else
            write_data(0x00);
            
        write_data(char_d[i]); //dds波形设置
        write_data(char_d[i]);
        write_data(char_s[i]);
        write_data(char_bo[(i<<1)]);
        write_data(char_bo[(i<<1)|0x01]);            
        write_data(char_xing[(i<<1)]);
        write_data(char_xing[(i<<1)|0x01]);   
        write_data(char_she[(i<<1)]);
        write_data(char_she[(i<<1)|0x01]);
        write_data(char_zhi[(i<<1)]);
        write_data(char_zhi[(i<<1)|0x01]);
            

        write_data(0x00);
                
    }
    if((LCD_Command>>7)==1)
    {
        for(i=0;i<16;i++)
        {
            switch(menu_p->menu_num)
            {
            case (1):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                
                write_data(char_shi1[(i<<1)]);  //示波器模式
                write_data(char_shi1[(i<<1)|0x01]);   
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);    
                write_data(char_qi[(i<<1)]);
                write_data(char_qi[(i<<1)|0x01]);  
                write_data(char_mo[(i<<1)]);
                write_data(char_mo[(i<<1)|0x01]);
                write_data(char_shi2[(i<<1)]);
                write_data(char_shi2[(i<<1)|0x01]);
                for(j=0;j<4;j++)
                    write_data(0x00);
                 
                 if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_pin[(i<<1)]);//频谱模式
                write_data(char_pin[(i<<1)|0x01]);   
                write_data(char_pu[(i<<1)]);
                write_data(char_pu[(i<<1)|0x01]);    
                write_data(char_mo[(i<<1)]);
                write_data(char_mo[(i<<1)|0x01]);
                write_data(char_shi2[(i<<1)]);
                write_data(char_shi2[(i<<1)|0x01]);
                for(j=0;j<36;j++)
                    write_data(0x00);
                
                break;
            case (2):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                
                write_data(char_shang[(i<<1)]);  //上升沿触发
                write_data(char_shang[(i<<1)|0x01]);   
                write_data(char_shen[(i<<1)]);
                write_data(char_shen[(i<<1)|0x01]);    
                write_data(char_yan[(i<<1)]);
                write_data(char_yan[(i<<1)|0x01]);  
                write_data(char_chu[(i<<1)]); 
                write_data(char_chu[(i<<1)|0x01]);
                write_data(char_fa[(i<<1)]);
                write_data(char_fa[(i<<1)|0x01]);
                for(j=0;j<4;j++)
                    write_data(0x00);
                 
                 if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_xia[(i<<1)]);  //下降沿触发
                write_data(char_xia[(i<<1)|0x01]);   
                write_data(char_jiang[(i<<1)]);
                write_data(char_jiang[(i<<1)|0x01]);    
                write_data(char_yan[(i<<1)]);
                write_data(char_yan[(i<<1)|0x01]);  
                write_data(char_chu[(i<<1)]); 
                write_data(char_chu[(i<<1)|0x01]);
                write_data(char_fa[(i<<1)]);
                write_data(char_fa[(i<<1)|0x01]);
                for(j=0;j<4;j++)
                    write_data(0x00);
                 
                if(menu_p->cursor==2)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_tiao[(i<<1)]);  //跳变沿触发
                write_data(char_tiao[(i<<1)|0x01]);   
                write_data(char_bian[(i<<1)]);
                write_data(char_bian[(i<<1)|0x01]);    
                write_data(char_yan[(i<<1)]);
                write_data(char_yan[(i<<1)|0x01]);  
                write_data(char_chu[(i<<1)]); 
                write_data(char_chu[(i<<1)|0x01]);
                write_data(char_fa[(i<<1)]);
                write_data(char_fa[(i<<1)|0x01]);

                for(j=0;j<19;j++)
                    write_data(0x00);
                break;
            case (3):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                
                write_data(char_guan[(i<<1)]);    //关闭kalman滤波
                write_data(char_guan[(i<<1)|0x01]); 
                write_data(char_bi[(i<<1)]);
                write_data(char_bi[(i<<1)|0x01]);                    
                write_data(char_k[i]); 
                write_data(char_a[i]);
                write_data(char_l[i]);
                write_data(char_m[i]);
                write_data(char_a[i]);
                write_data(char_n[i]);
                write_data(char_lv2[(i<<1)]);
                write_data(char_lv2[(i<<1)|0x01]);
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);  

                 
                if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_kai[(i<<1)]);    //开启kalman滤波
                write_data(char_kai[(i<<1)|0x01]);   
                write_data(char_qi2[(i<<1)]);
                write_data(char_qi2[(i<<1)|0x01]);               
                write_data(char_k[i]); 
                write_data(char_a[i]);
                write_data(char_l[i]);
                write_data(char_m[i]);
                write_data(char_a[i]);
                write_data(char_n[i]);
                write_data(char_lv2[(i<<1)]);
                write_data(char_lv2[(i<<1)|0x01]);
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);
                
                for(j=0;j<30;j++)
                    write_data(0x00);
                break;
            case (4):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                
                write_data(char_guan[(i<<1)]);    //关闭滤波
                write_data(char_guan[(i<<1)|0x01]); 
                write_data(char_bi[(i<<1)]);
                write_data(char_bi[(i<<1)|0x01]);                    
                write_data(char_lv2[(i<<1)]);
                write_data(char_lv2[(i<<1)|0x01]);
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);
                write_data(0x00);
                write_data(0x00);
                write_data(0x00);
                write_data(0x00);
                write_data(0x00);
                write_data(0x00);
                 
                if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_kai[(i<<1)]);    //开启高通滤波
                write_data(char_kai[(i<<1)|0x01]); 
                write_data(char_qi2[(i<<1)]);
                write_data(char_qi2[(i<<1)|0x01]);                    
                write_data(char_gao[(i<<1)]);
                write_data(char_gao[(i<<1)|0x01]);
                write_data(char_tong[(i<<1)]);
                write_data(char_tong[(i<<1)|0x01]);  
                write_data(char_lv2[(i<<1)]);
                write_data(char_lv2[(i<<1)|0x01]);
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);  
                write_data(0x00);
                write_data(0x00);
 
                if(menu_p->cursor==2)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_kai[(i<<1)]);    //开启低通滤波
                write_data(char_kai[(i<<1)|0x01]); 
                write_data(char_qi2[(i<<1)]);
                write_data(char_qi2[(i<<1)|0x01]);                    
                write_data(char_di[(i<<1)]);
                write_data(char_di[(i<<1)|0x01]);
                write_data(char_tong[(i<<1)]);
                write_data(char_tong[(i<<1)|0x01]);  
                write_data(char_lv2[(i<<1)]);
                write_data(char_lv2[(i<<1)|0x01]);
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);  


                for(j=0;j<17;j++)
                    write_data(0x00);
                break;
                
            case (5):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                write_data(char_jie[(i<<1)]);//解锁显示
                write_data(char_jie[(i<<1)|0x01]);                 
                write_data(char_suo[(i<<1)]);
                write_data(char_suo[(i<<1)|0x01]);              
                write_data(char_chu[(i<<1)]); 
                write_data(char_chu[(i<<1)|0x01]);
                write_data(char_fa[(i<<1)]);
                write_data(char_fa[(i<<1)|0x01]);                
                
                

                for(j=0;j<6;j++)
                    write_data(0x00);
                 
                if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                
                write_data(char_suo[(i<<1)]);//锁定触发
                write_data(char_suo[(i<<1)|0x01]);
                write_data(char_ding[(i<<1)]);
                write_data(char_ding[(i<<1)|0x01]);               
                write_data(char_chu[(i<<1)]); 
                write_data(char_chu[(i<<1)|0x01]);
                write_data(char_fa[(i<<1)]);
                write_data(char_fa[(i<<1)|0x01]);                


                for(j=0;j<36;j++)
                    write_data(0x00);
                break;
            case (6):
                if(menu_p->cursor==0)
                    write_data(char_select[i]);
                else
                    write_data(0x00);
                
                write_data(char_fang[(i<<1)]);  //方波
                write_data(char_fang[(i<<1)|0x01]);   
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);    
                for(j=0;j<10;j++)
                    write_data(0x00);
                 
                 if(menu_p->cursor==1)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_zheng[(i<<1)]);  //正弦波
                write_data(char_zheng[(i<<1)|0x01]);   
                write_data(char_xian2[(i<<1)]);
                write_data(char_xian2[(i<<1)|0x01]);    
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);  
                for(j=0;j<8;j++)
                    write_data(0x00);
                 
                if(menu_p->cursor==2)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_jia[(i<<1)]); //+随机噪声
                write_data(char_jia[(i<<1)|0x01]);   
                write_data(char_sui[(i<<1)]); 
                write_data(char_sui[(i<<1)|0x01]);   
                write_data(char_ji[(i<<1)]);
                write_data(char_ji[(i<<1)|0x01]);    
                write_data(char_zao[(i<<1)]);
                write_data(char_zao[(i<<1)|0x01]);  
                write_data(char_sheng[(i<<1)]); 
                write_data(char_sheng[(i<<1)|0x01]);
                for(j=0;j<4;j++)
                    write_data(0x00);
                
                if(menu_p->cursor==3)
                    write_data(char_select[i]); 
                else
                    write_data(0x00);
                write_data(char_he[(i<<1)]);  //合成波
                write_data(char_he[(i<<1)|0x01]);   
                write_data(char_cheng[(i<<1)]);  
                write_data(char_cheng[(i<<1)|0x01]);   
                write_data(char_bo[(i<<1)]);
                write_data(char_bo[(i<<1)|0x01]);    
                for(j=0;j<8;j++)
                    write_data(0x00);
                break;
                break;
            default:
                break;
            }
              
        }
        
        

    }
    else;
}


void dds_data(uint8 dds_num)
{
    uint16 i;
    write_command(clr_dds_add);
    write_command(write_dds_ram);
    
    switch(dds_num)
    {
        case(0):
            for(i=0;i<256;i++)
                write_data(pwm_wave[0]);
            for(i=0;i<256;i++)
                write_data(pwm_wave[1]);   
            break;
        case(1):
        for(i=1;i<512;i++)
            write_data(Sine_wave[i]);
            break;
        case(2):
        for(i=1;i<512;i++)
            write_data(noise_wave[i]);
            break;
        case(3):
        for(i=1;i<512;i++)
            write_data(mix_wave[i]);
            break;
        default:
            break;
        
    }
}

void Set_fre_div(uint8 fre_div)
{
    Fre_wirte(0);
    set_add(fre_div);
    Delay_cnt(3);
    Fre_wirte(1);
}

uint16 trigger_seek(void)
{
    uint16 add;
    uint8 last_sample_data,sample_data;
    uint8 PointCnt1=0;
    uint8 PointCnt2=0;
    uint8 PointCnt3=0;
    uint8 PointCnt4=0;
    
    sample_data = read_data(0x0000);
    for(add=1;add<8192;add++)
    {
        last_sample_data = sample_data;
        sample_data = read_data(add);
        switch(trigger.value)
        {
            case(0):
                if(PointCnt1>Trigger_Num)
                {
                    return (add-Trigger_Num);
                }
                else if(PointCnt1&&(sample_data>=last_sample_data))
                    PointCnt1++;
                else if((!PointCnt1)&&(sample_data>TriggerPoint)&&(last_sample_data<TriggerPoint))
                    PointCnt1++;
                else
                    PointCnt1==0;
                
                break;
            case(1):
                if(PointCnt2>Trigger_Num)
                {
                    return (add-Trigger_Num);
                }
                else if(PointCnt2&&(sample_data<=last_sample_data))
                    PointCnt2++;
                else if((!PointCnt2)&&(sample_data<TriggerPoint)&&(last_sample_data>TriggerPoint))
                    PointCnt2++;
                else
                    PointCnt2==0;
                break;
            case(3):
                if(PointCnt3>Trigger_Num)
                {
                    return (add-Trigger_Num);
                }
                else if(PointCnt3&&(sample_data<=last_sample_data))
                    PointCnt3++;
                else if((!PointCnt3)&&(sample_data<TriggerPoint)&&(last_sample_data>TriggerPoint))
                    PointCnt3++;
                else
                    PointCnt3==0;
                
                if(PointCnt4>Trigger_Num)
                {
                    return (add-Trigger_Num);
                }
                else if(PointCnt4&&(sample_data<=last_sample_data))
                    PointCnt4++;
                else if((!PointCnt4)&&(sample_data<TriggerPoint)&&(last_sample_data>TriggerPoint))
                    PointCnt4++;
                else
                    PointCnt4==0;
                break;
            default:
                break;
		}
	}
      
	return (0);
}

void show_para(uint8 * ParaData)
{
    uint32 i;
    uint8 tem1,tem2,tem3;
    
    LCD_Command = (menu_p->state)?(0x80+clr_para_add):clr_para_add;
    write_command(LCD_Command);//清除cnt
    
    LCD_Command = (menu_p->state)?(0x80+para_ram):para_ram;
    write_command(LCD_Command);
        
    tem1 = (ParaData[0]+1)/100;
    tem2 = (ParaData[0]+1)%100/10;
    tem3 = (ParaData[0]+1)%10;
    for(i=0;i<16;i++)
    {
        write_data(char_num[5][i]);
        write_data(char_num[0][i]);
        write_data(char_M[i]);
        write_data(char_slash[i]);
        
        if(tem1)
            write_data(char_num[tem1][i]);
        else
            write_data(0x00);
        if(tem2)
            write_data(char_num[tem2][i]);
        else if(tem1)
            write_data(char_num[0][i]);
        else
            write_data(0x00);
        if(tem3)
            write_data(char_num[tem3][i]);
        else
            write_data(char_num[0][i]);
    }
    tem1 = (ParaData[1])/10;
    tem2 = (ParaData[1])%10;
    for(i=0;i<16;i++)
    {
        if(tem1)
            write_data(char_num[tem1][i]);
        else
            write_data(0x00);
        if(tem2)
            write_data(char_num[tem2][i]);
        else
            write_data(char_num[0][i]);
        write_data(char_slash[i]);
        write_data(char_num[1][i]);
        write_data(char_num[6][i]);
        write_data(0x00);
        write_data(0x00);
    }
    tem1 = (ParaData[2])/100;
    tem2 = (ParaData[2])%100/10;
    tem3 = (ParaData[2])%10;    
    for(i=0;i<16;i++)
    {
        write_data(char_jia[(i<<1)]);
        write_data(char_jia[(i<<1)+1]);
        if(tem1)
            write_data(char_num[tem1][i]);
        else
            write_data(0x00);
        if(tem2)
            write_data(char_num[tem2][i]);
        else if(tem1)
            write_data(char_num[0][i]);
        else
            write_data(0x00);
        if(tem3)
            write_data(char_num[tem3][i]);
        else
            write_data(char_num[0][i]);
        write_data(0x00);
        write_data(0x00);
    }
    for(i=0;i<16;i++)
    {
        write_data(char_num[5][i]);
        write_data(char_num[5][i]);
        write_data(char_slash[i]);
        write_data(char_num[1][i]);
        write_data(char_num[6][i]);
        write_data(0x00);
        write_data(0x00);
    }
}