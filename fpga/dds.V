module dds(
	clk,
	rst,
	dds_data_get,
	dds_out,
	dds_cnt,
	dds_clk
	);
input clk,rst;
input [7:0]dds_data_get;
output [7:0]dds_out;
output [8:0]dds_cnt;
output dds_clk;

reg [7:0]dds_clk_cnt;
reg dds_clk;

parameter dds_clk_time = 24;



always @(posedge clk or negedge rst) begin
	if(!rst) begin
		dds_clk_cnt <=8'd0;
		dds_clk <= 1'b0;
	end
	else if(dds_clk_cnt >=dds_clk_time) begin
		dds_clk_cnt <=0;
		dds_clk <=!dds_clk;
	end	
	else begin
		dds_clk_cnt <=dds_clk_cnt+1'b1;
	end
end

reg [8:0]dds_cnt;
reg [7:0]dds_out;

//datasheet显示DA上升沿写入数据 下降沿改变数据 以避免亚稳态
always @(negedge dds_clk or negedge rst) begin
	if(!rst) begin
		dds_cnt <=0;
	end
	else begin
		dds_cnt <= dds_cnt+1'b1;
		//dds_out <= dds_data_get;
		dds_out <= (8'hff-dds_data_get);
	end
end

endmodule