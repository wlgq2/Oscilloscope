`define show_select (get_ram_data)&&(v_state_cnt<(v_back_time+16))&&((h_state_cnt<=8)||((h_state_cnt<=80)&&(h_state_cnt>72))||((h_state_cnt<=152)&&(h_state_cnt>144))||((h_state_cnt<=240)&&(h_state_cnt>232))||((h_state_cnt<=312)&&(h_state_cnt>304))||((h_state_cnt<=384)&&(h_state_cnt>376)))
`define show_select_2 (get_ram_data)&&(v_state_cnt>=(16+v_back_time))&&((h_state_cnt<=8)||((h_state_cnt<=128)&&(h_state_cnt>120))||((h_state_cnt<=248)&&(h_state_cnt>240))||((h_state_cnt<=368)&&(h_state_cnt>360)))

`define h_trie_show (h_state_cnt==9'd31)||(h_state_cnt==9'd63)||(h_state_cnt==9'd95)||(h_state_cnt==9'd127)||(h_state_cnt==9'd159)||(h_state_cnt==9'd191)||(h_state_cnt==9'd223)||(h_state_cnt==9'd256)||(h_state_cnt==9'd288)||(h_state_cnt==9'd319)||(h_state_cnt==9'd351)||(h_state_cnt==9'd383)||(h_state_cnt==9'd415)||(h_state_cnt==9'd447)
`define v_trie_show (v_state_cnt==70)||(v_state_cnt==102)||(v_state_cnt==134)||(v_state_cnt==166)||(v_state_cnt==198)||(v_state_cnt==230)||(v_state_cnt==263)
`define show_wave (((9'd293-v_state_cnt)>last_get_show_data)&&((9'd293-v_state_cnt)<=get_show_data))||(((9'd293-v_state_cnt)<last_get_show_data)&&((9'd293-v_state_cnt)>=get_show_data))||((9'd293-v_state_cnt)==get_show_data)
//`define show_wave (((9'd293-v_state_cnt)>(get_show_data-2))&&((9'd293-v_state_cnt)<=get_show_data))||(((9'd293-v_state_cnt)==get_show_data))



module lcd_show(clk,rst,wr,oe,data,
             lcd_r,lcd_g,lcd_b,lcd_clk,
				 lcd_hsync,lcd_vsync,lcd_de,
				 lcd_pwm,
				 
				 
				 get_ram_add,
				 get_ram_data,
				 get_show_data,
				 get_show_data_cnt,
				 ram_commond,
				 
				 para_ram_add,
				 para_ram_data
				 );
				 
				 //
				 


input clk,rst,oe,wr;
input data;
input para_ram_data;
output [11:0]para_ram_add;
input [7:0]ram_commond;
output [7:0] lcd_r,lcd_g,lcd_b;
output lcd_clk,lcd_hsync,lcd_vsync,lcd_de,lcd_pwm;

input [7:0]get_ram_data;
output [13:0]get_ram_add;
input [7:0]get_show_data;
output [8:0]get_show_data_cnt;
(* KEEP ="TRUE" *)wire lcd_clk;
reg [7:0]lcd_r,lcd_g,lcd_b;
reg lcd_hsync,lcd_vsync,lcd_de;

//reg [3:0]led;
reg [8:0]get_show_data_cnt;
reg [7:0]current_state,next_state,v_state;


reg lcd_clk_odd1;
reg lcd_clk_odd2;

reg [7:0]lcd_cnt;
reg [9:0]h_state_cnt,v_state_cnt;

reg lcd_ret_flg;
reg [1:0]hsync_init_flg;

//parameter switch_time =32'd2000;
parameter lcd_pwm_duty =  8'd80;
//10mhz
parameter lcd_clk_period = 8'd5;


/************state******************/
parameter init            = 8'd4;
parameter h_sync          = 8'd0;
parameter h_back_porch    = 8'd1;
parameter h_active_video  = 8'd2;
parameter h_front_porch   = 8'd3;



/************state time*************/
parameter h_sync_time     = 8'd1;
parameter h_back_time     = 8'd43;
parameter h_active_time   = 10'd480;
parameter h_front_time    = 8'd8;

parameter h_period        = 10'd531;

parameter v_sync_time     = 10'd10;
parameter v_back_time     = 10'd22;
parameter v_active_time   = 10'd294;
parameter v_front_time    = 10'd298;


assign lcd_clk = lcd_clk_odd2|lcd_clk_odd1;


always@(posedge clk)
begin
	if(!rst)
	begin
		lcd_cnt <=0;
		lcd_clk_odd1 <=0;
		lcd_ret_flg <=1;
	end
	else if(lcd_cnt >=(lcd_clk_period-1))
	begin
		lcd_cnt <=0;
		lcd_clk_odd1 <=1;
		lcd_ret_flg <=0;
	end
	else if(lcd_cnt >=((lcd_clk_period-3)>>1))
	begin
    lcd_cnt <=lcd_cnt+8'd1;
    lcd_clk_odd1 <=0;
	end
	else 
	begin
    lcd_cnt <=lcd_cnt+8'd1;
    lcd_clk_odd1 <=1;
	end
end

always@(negedge clk)
begin

    lcd_clk_odd2 <= lcd_clk_odd1;
end


//初始state相关
always@(posedge lcd_clk)
begin
	if(lcd_ret_flg)
	begin
		current_state <= init;
		
	end
	else
	begin
		current_state <=next_state;
	end
end


reg [7:0]lcd_pwm_cnt;
reg lcd_pwm;
//分频提供LCD PWM时钟
always@(posedge lcd_clk)
begin
	if(lcd_ret_flg)
	begin
		lcd_pwm_cnt <= 8'b0;
		lcd_pwm <= 1'b0;
		
	end
	else if(lcd_pwm_cnt>lcd_pwm_duty)
	begin 
		lcd_pwm <= 1;
		lcd_pwm_cnt <=lcd_pwm_cnt+1;
	end
	else
	begin
		lcd_pwm <= 0;
		lcd_pwm_cnt <=lcd_pwm_cnt+1;
	end
end

//状态机转移
always@(negedge lcd_clk)
begin
	case(current_state)
		init:
		begin
			h_state_cnt <=0;
			next_state <=h_sync;
			
			hsync_init_flg <=2;
		end
		h_sync:
		begin
			if(h_state_cnt>=(h_sync_time-1))
			begin
				next_state <= h_back_porch;
				h_state_cnt <=0;
			end
			else
			begin
				next_state <= h_sync;
				h_state_cnt <= h_state_cnt+1;
			end
		end
		h_back_porch:
		begin
			if(h_state_cnt>=(h_back_time-1))
			begin
				next_state <= h_active_video;
				h_state_cnt <=0;
			end
			else
			begin
				next_state <= h_back_porch;
				h_state_cnt <= h_state_cnt+1;
			end
		end
		h_active_video:
		begin
			if(h_state_cnt>=(h_active_time-1))
			begin
				next_state <=h_front_porch;
				h_state_cnt <=0;
			end
			else
			begin
				next_state <=h_active_video;
				h_state_cnt <= h_state_cnt +1;
			end
		end
		h_front_porch:
		begin
			if(h_state_cnt >=(h_front_time-1))
			begin
				next_state<=h_sync;
				hsync_init_flg <= hsync_init_flg>>1;
			end
			else
			begin
				h_state_cnt <=h_state_cnt +1;
				next_state<=h_front_porch;
			end
		end
	endcase
end

reg [13:0]get_ram_add;
reg [13:0]end_show_add;
wire  get_ram_data;
//状态机输出

reg [11:0]para_ram_add;

reg [7:0]last_get_show_data;
always@(posedge lcd_clk)
begin
	case(current_state)
		init:
		begin
			lcd_hsync <=0;

			
			lcd_de <= 0;
			/*lcd_r <= 8'd0;
			lcd_g <= 8'd0;
			lcd_b <= 8'd0;*/
		end
		h_sync:
		begin
			lcd_hsync <=0;
			
			/*lcd_r <= 8'd0;
			lcd_g <= 8'd0;
			lcd_b <= 8'd0;*/
			lcd_de <=0;
			//if(v_state_cnt <v_back_time)
				//get_ram_add <=0;
		
			get_show_data_cnt <=9'd0;
			
			//if(v_state_cnt<(v_back_time+32)) begin
				//para_ram_add <= 9'd0;
			//end
		//	else begin 
		//	end
		end
		h_back_porch:
		begin
			lcd_hsync <=1;
			if((ram_commond[7]) == 0) 
				end_show_add <=14'd7679;
			else if((ram_commond[7]) == 1)
				end_show_add <=14'd15359;
			else
			begin
			end
			
		end
		h_active_video:
		begin		
			show_task;
		end
		h_front_porch:
		begin
			lcd_de <=0;
			/*lcd_r <= 8'h00;
			lcd_g <= 8'h00;
			lcd_b <= 8'h00;*/
		end
		default:
		begin
		end
	endcase
end

always @(negedge lcd_hsync)
begin
	if(hsync_init_flg>0)
		v_state_cnt<=0;
	else if(v_state_cnt>=(v_front_time-1))
	begin
		lcd_vsync <=0;
		v_state_cnt<=0;
	end
	else if(v_state_cnt>=(v_sync_time-1))
	begin
		v_state_cnt <=v_state_cnt+1;
		lcd_vsync <=1;
		
	end
	else
		v_state_cnt<=v_state_cnt+1;
end



/*菜单显示啦~*/
task show_task;
begin
	if((v_state_cnt >=v_back_time)&&(v_state_cnt <v_active_time))begin
		if(get_ram_add >end_show_add)begin
			show_data;
		end
		else begin
			get_ram_add <= get_ram_add+1;
			lcd_de <=1;
			
			if(`show_select) begin
				lcd_r[7] <=1;
				lcd_g[7] <=0;
				lcd_b[7] <=0;
			
			end
			else if(`show_select_2) begin
				lcd_r[7] <=1;
				lcd_g[7] <=0;
				lcd_b[7] <=0;
			end
			else if((get_ram_data)&&(v_state_cnt<(v_back_time+16)))begin
				lcd_r[7] <=0;
				lcd_g[7] <=0;
				lcd_b[7] <=1;
			end
			else if(get_ram_data) begin
				lcd_r[7] <=0;
				lcd_g[7] <=1;
				lcd_b[7] <=0;
			end
			else begin
				lcd_r[7] <=0;
				lcd_g[7] <=0;
				lcd_b[7] <=0;
			end
		end
	end
	else
	begin
		lcd_de <=0;
		get_ram_add  <=0;
	end
end
endtask

/*显示波形啦*/
`define para_ram (v_state_cnt<(v_back_time+97))&&(v_state_cnt>(v_back_time+32))&&(h_state_cnt<58)&&(h_state_cnt>1)
`define show_para para_ram_data
task show_data; begin
	lcd_de <=1;
	if((`para_ram)&&(`show_para)) begin  //显示各种参数
		lcd_r[7] <=1;
		lcd_g[7] <=0;
		lcd_b[7] <=1;
		//para_ram_add <= para_ram_add+1'b1;
		
		last_get_show_data <=get_show_data;
		get_show_data_cnt <=get_show_data_cnt+1'd1;		
	end
	else if(`show_wave) begin
		lcd_r[7] <=0;
		lcd_g[7] <=1;
		lcd_b[7] <=1;
		last_get_show_data <=get_show_data;
		get_show_data_cnt <=get_show_data_cnt+1'd1;
	end
	else if((`h_trie_show)||(`v_trie_show)) begin
		lcd_r[7] <=1;
		lcd_g[7] <=1;
		lcd_b[7] <=0;
		last_get_show_data <=get_show_data;
		get_show_data_cnt <=get_show_data_cnt+1'd1;
	end
	else begin
		lcd_r[7] <=0;
		lcd_g[7] <=0;
		lcd_b[7] <=0; 
		last_get_show_data <=get_show_data;
		get_show_data_cnt <=get_show_data_cnt+1'd1;
	end
	
	if(`para_ram) begin
		para_ram_add <= para_ram_add+1'b1;
		
		//last_get_show_data <=get_show_data;
		//get_show_data_cnt <=get_show_data_cnt+1'd1;			
	end
	else if(v_state_cnt>(v_back_time+110))begin
		para_ram_add <= 9'd0;
	end
	else begin
	end
end
endtask
endmodule