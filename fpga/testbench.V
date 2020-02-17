module test;

parameter period=10;
reg CLK,RST,OE;
reg[7:0]DATAADD,DATA;

wire LCD_DI,LCD_RW,LCD_PS,LCD_EN;
wire [7:0] LCD_DATA;
wire [10:0] STATEOUT;

wire [3:0] LED;


top top_simu(.clk(CLK),
             .rst(RST),
             .wr(),
             .oe(),
             .mcu_data(),
             .lcd_r(),
             .lcd_g(),
             .lcd_b(),
             .lcd_clk(),
				     .lcd_hsync(),
				     .lcd_vsync(),
				     .lcd_de(),
				     .lcd_pwm(), 
				     .led()
				 );
			
				
initial begin 
	CLK <=0;
	forever #period CLK =~CLK;
end

initial begin 
	rst_task(100);
	#100;
	rst_task(400);
end


task rst_task;
input[15:0]rst_time;
begin
	RST <=0;
	#rst_time;
	RST <=1;
end
endtask

task error;
input[100:0]msg;
begin
	$write("ERROR at%t:%s",$time,msg);
end
endtask

task warning;
input[100:0]msg;
begin
	$write("WARNING at%t:%s",$time,msg);
end
endtask

task terminate;
begin 
  $display("--END---");
end
endtask

endmodule