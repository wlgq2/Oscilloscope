module ram_write(clk,rst,wr,oe,mcu_data,
					add_a,data_a,
					wr_clk,Ram_command,
					 show_data,
					 show_data_cnt,
					 dds_w_data,
					 dds_w_add,
					 para_w_data,
					 para_w_add
				 );
input clk,rst,wr,oe;
input [7:0]mcu_data;
input [8:0]show_data_cnt;
output [10:0]add_a;
output [7:0]data_a,Ram_command,show_data,dds_w_data,para_w_data;
output [8:0]dds_w_add,para_w_add;
output wr_clk;


reg [10:0]add_a;
reg [7:0]Ram_command,show_data,data_a;

reg wr1,wr2;

reg [8:0] show_data_ram_cnt;

reg [7:0] show_data_ram[479:0];

assign wr_clk = wr2;

//command[7]  1:显示下拉  0：不显示下拉
//command[6]  1:显示频谱选择 0：不显示选择
parameter clr_menu_cnt = 8'd0; //清除显示RAM计数
parameter menu_ram = 8'd2; //菜单RAM	区
parameter dds_ram = 8'd3;
parameter show_data_command = 8'd1;
parameter clr_data_cnt = 8'd4; //清除波形数据计数
parameter clr_dds_add = 8'd5; //清除dds数据计数

parameter clr_para_add = 8'd6;  
parameter para_ram =8'd7;    //采样频率，扫描时间 相位等参数

parameter para_fir_h = 8'd8;
parameter para_fir_l = 8'd9;

reg [8:0]dds_w_add,para_w_add;
reg [7:0]dds_w_data,para_w_data;

always@(posedge wr)
begin
	case(Ram_command[6:0])
		clr_menu_cnt: begin
			add_a <= 10'd0-1'b1;  //因为打拍了，所以add必须这样
		end
		clr_data_cnt: begin
			show_data_ram_cnt <=9'd0;
		end
		menu_ram: begin
			add_a <= add_a+1'b1;
			data_a <= mcu_data;	
		end
		show_data_command: begin
			show_data_ram_cnt <=show_data_ram_cnt+1'b1;
			show_data_ram[show_data_ram_cnt] <= mcu_data;
		end
		clr_dds_add: begin
			dds_w_add <= 9'd0-1'b1;
			//dds_w_add <= 9'd2;
		end
		dds_ram: begin
			dds_w_add <= dds_w_add+1'b1;
			//dds_w_add <= 9'h1f1;
			dds_w_data <= mcu_data;
			//dds_w_data <= 8'h55;
		end
		clr_para_add: begin
			para_w_add <= 9'd0-1'b1;
		end
		para_ram: begin
			para_w_add <= para_w_add+1'b1;
			para_w_data <= mcu_data;
		end
	endcase

	if(oe) begin
		Ram_command <=mcu_data;
		
	end
end

always@(posedge clk) begin
	wr1 <=wr;
	wr2 <=wr1;
	show_data <=show_data_ram[show_data_cnt];
end
endmodule