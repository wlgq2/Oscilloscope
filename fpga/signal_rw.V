module signal_rw(
	clk,
	rst,
	mcu_r_start,
	mcu_r_add,
	mcu_r_data,
	signal,
	signal_clk_out,
	cpu_w_div
	); 
input clk,rst,mcu_r_start;
input cpu_w_div;
input [7:0]signal;
input [12:0]mcu_r_add;
output [7:0]mcu_r_data;
output signal_clk_out;


//分频
reg [8:0]Fre_div;
always @(posedge cpu_w_div) begin
	Fre_div[8:0] <= mcu_r_add[8:0];
end

reg [8:0]signal_cnt;
reg signal_clk;
always @(posedge clk or negedge rst) begin
	if(!rst) begin
		signal_cnt <= 13'd0;
		signal_clk <= 1'b0;
	end
	else if(signal_cnt >=(Fre_div-1'b1)) begin
		signal_cnt <=0;
		signal_clk <=!signal_clk;
	end	
	else begin
		signal_cnt <=signal_cnt+1'b1;
	end
end
wire signal_clk_out;
//构建2的n次方分频
assign signal_clk_out = (Fre_div)?signal_clk:clk;
//assign signal_clk_out = clk;


reg mcu_r_start_state,last_mcu_start_state;
always @(posedge signal_clk_out) begin
	mcu_r_start_state <=mcu_r_start;
	last_mcu_start_state <=mcu_r_start_state;
end

wire mcu_r_start_flag;
assign mcu_r_start_flag = (!last_mcu_start_state)&mcu_r_start_state;

reg [12:0]RAM1_W_add,RAM2_W_add;

//构建 signal与 RAM1,RAM2,信号切换
wire [7:0]RAM1_W_data,RAM2_W_data;

wire [7:0]RAM1_R_data,RAM2_R_data;
wire [12:0]RAM1_R_add,RAM2_R_add;

reg ram_switch;

//构建mcu data,add与 RAM1,RAM2,信号切换
assign RAM1_R_add = ram_switch? mcu_r_add:13'h0;      //mcu_add :0
assign RAM2_R_add = ram_switch? 13'h0:mcu_r_add;      //mcu_add :0
assign mcu_r_data = ram_switch? RAM1_R_data:RAM2_R_data;//RAM1DATA:RAM2DATA



assign RAM1_W_data = ram_switch? 8'd0:signal;   //sianal:0
assign RAM2_W_data = ram_switch? signal:8'd0;   //signal:0


wire wea;
assign wea = 1;
wire RAM1_w_clk,RAM2_w_clk;
assign RAM1_w_clk = ram_switch? 1'd0:(!signal_clk_out);  //!signal_clk_out:0
assign RAM2_w_clk = ram_switch? (!signal_clk_out):1'd0;  //!signal_clk_out:0

//调RAM啊
signal_ram RAM1(
  .clka(RAM1_w_clk),
  .wea(wea),
  .addra(RAM1_W_add),
  .dina(RAM1_W_data),
  .clkb(clk),
  .addrb(RAM1_R_add),
  .doutb(RAM1_R_data)
);

signal_ram RAM2(
  .clka(RAM2_w_clk),
  .wea(wea),
  .addra(RAM2_W_add),
  .dina(RAM2_W_data),
  .clkb(clk),
  .addrb(RAM2_R_add),
  .doutb(RAM2_R_data)
);

parameter idle = 4'd0;
parameter RAM1_W = 4'd1;
parameter RAM2_W = 4'd2;

reg [3:0]state;

always @(negedge signal_clk_out or negedge rst) begin
	if(!rst) begin
		state <=idle;
		ram_switch<=0;
	end
	else if (state ==idle) begin
		if(mcu_r_start_flag&(!ram_switch))//检测起始标志和switch,RAM1,2,
			state <= RAM1_W;
		else if(mcu_r_start_flag&ram_switch)
			state <= RAM2_W;
		else begin
		end
	end
	else if(state == RAM1_W) begin
		//仿真需要改，
		//if(RAM1_W_add==13'h1f) begin
		if(RAM1_W_add==13'h1fff) begin
			state <=idle;
			ram_switch <= !ram_switch;
		end
		else begin
		end
	end
	else if(state == RAM2_W) begin
		//仿真需要改，
		//if(RAM2_W_add==13'h1f) begin
		if(RAM2_W_add==13'h1fff) begin
			state <=idle;
			ram_switch <= !ram_switch;
		end
		else begin
		end
	end
	else begin
	end
end

always @(posedge signal_clk_out) begin
	if (state ==idle) begin
		RAM2_W_add <= 13'd0;
		RAM1_W_add <= 13'd0;
	end
	else if(state == RAM1_W) begin
		RAM1_W_add <=RAM1_W_add+1'd1;
	end
	else if(state == RAM2_W) begin
		RAM2_W_add <=RAM2_W_add+1'd1;
	end
	else begin
	end
end


endmodule