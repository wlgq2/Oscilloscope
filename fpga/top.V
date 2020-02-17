module top(
	clk,
	rst,
	wr,
	oe,
	mcu_data,
   lcd_r,
	lcd_g,
	lcd_b,
	lcd_clk,
	lcd_hsync,
	lcd_vsync,
	lcd_de,
	lcd_pwm, 
	//led,
	DDS_OUT,
	DDS_CLK,
	

	cpu_r_data,   //CPU读取RAM数据
	cpu_r_add,  //CPU读取RAM地址
	cpu_r_start,  //CPU读取起始信号
	
	SIGNAL_CLK, //输出给采集时钟
	SIGNAL,     //signal信号
	
	CPU_W_DIV  //输入分频值
 );
				 
input clk,rst,oe,wr,cpu_r_start;
input [12:0]cpu_r_add;
input CPU_W_DIV;
input [7:0]mcu_data,SIGNAL;
output [7:0] lcd_r,lcd_g,lcd_b,DDS_OUT,cpu_r_data;
output lcd_clk,lcd_hsync,lcd_vsync,lcd_de,lcd_pwm,DDS_CLK,SIGNAL_CLK;
//output [3:0] led;




wire get_ram_data;
wire [13:0]get_ram_add;
wire [8:0]SHOW_DATA_CNT;
wire [7:0]SHOW_DATA;

wire [7:0]RAM_COMMAND;




				 
				 
				 
wire wea;
wire [10:0]addra;
wire [7:0]din_a;

/*reg [7:0]top_clk_cnt;
reg top_clk;
parameter top_time_cnt = 8'd10;

*/
assign wea =1;

show_ram get_data(
  .clka(wr_out),
  .wea(wea),
  .addra(addra),
  .dina(din_a),
  .clkb(clk),
  .addrb(get_ram_add),
  .doutb(get_ram_data)
);  

wire [8:0]ddsw_add;
wire [8:0]ddsr_add;
wire [7:0]ddsw_data;
wire [7:0]ddsr_data;


//assign led[0] = ddsr_data[4];
//assign led[1] = ddsr_data[5];
//assign led[2] = ddsr_data[6];
//assign led[3] = ddsr_data[7];

dds_ram my_ddsram( 
  .clka(wr_out),
  .wea(wea),
  .addra(ddsw_add),
  .dina(ddsw_data),
  .clkb(clk),
  .addrb(ddsr_add),
  .doutb(ddsr_data)
);

wire DDS_CLK;
wire [7:0]DDS_OUT;

dds my_dds(
	.clk(clk),
	.rst(rst),
	.dds_data_get(ddsr_data),
	.dds_out(DDS_OUT),
	.dds_cnt(ddsr_add),
	.dds_clk(DDS_CLK)
	);
	
	
wire [7:0]paraw_data;
wire [8:0]paraw_add;

wire [11:0]parar_add;
wire parar_data;
ram_write ram_w(.clk(clk),
					 .rst(rst),
					 .wr(wr),
					 .oe(oe),
					 .mcu_data(mcu_data),
					 .add_a(addra),
					 .data_a(din_a),
					 .wr_clk(wr_out),
					 .Ram_command(RAM_COMMAND),
					 .show_data(SHOW_DATA),
					 .show_data_cnt(SHOW_DATA_CNT),
					 .dds_w_data(ddsw_data),
					 .dds_w_add(ddsw_add),
					 
					 .para_w_data(paraw_data),
					 .para_w_add(paraw_add)
				 );

wire CPU_W_DIV;
signal_rw signal_ctrl(
	.clk(clk),
	.rst(rst),
	.mcu_r_start(cpu_r_start),
	.mcu_r_add(cpu_r_add),
	.mcu_r_data(cpu_r_data),
	.signal(SIGNAL),
	.signal_clk_out(SIGNAL_CLK),
	.cpu_w_div(CPU_W_DIV)
	); 
	
	
	
wire wocao;
assign wocao = 9'd1;
wire [8:0]woqu;
assign woqu = 9'hff;

para_ram para_rw(
  .clka(wr_out),
  .wea(wea),
  .addra(paraw_add),
  .dina(paraw_data),
  .clkb(clk),
  .addrb(parar_add),
  .doutb(parar_data)
);

lcd_show lcd_ctrl(.clk(clk),
                .rst(rst),
					 .wr(wr),
					 .oe(oe),
					 .data(data),
                .lcd_r(lcd_r),
					 .lcd_g(lcd_g),
					 .lcd_b(lcd_b),
					 .lcd_clk(lcd_clk),
					 .lcd_hsync(lcd_hsync),
					 .lcd_vsync(lcd_vsync),
					 .lcd_de(lcd_de),
					 .lcd_pwm(lcd_pwm),
					 
					 .get_ram_add(get_ram_add),
					 .get_ram_data(get_ram_data),
					 .ram_commond(RAM_COMMAND),
					 .get_show_data(SHOW_DATA),
					 .get_show_data_cnt(SHOW_DATA_CNT),
					 
					 .para_ram_add(parar_add),
				    .para_ram_data(parar_data)
				 );
endmodule