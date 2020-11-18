module state_machine (
input clk, rst,
input [3:0] A,
input [3:0] B,
output reg [1:0] x,
output reg y);

parameter s0 = 2'b0;
parameter s1 = 2'b1;
parameter s2 = 2'b10;
reg [1:0] state, next_state;

initial begin
  state=0;
end

always @ (posedge clk or rst)
  begin
  if (rst) state <= s0;
  else state <= next_state;
  end

always @ (state, A, B)
begin
 case(state)
s0: 
if ((A == 10 ) & ( B == 9))
  next_state <= s1;
s1: 
  next_state <= s2;
s2: 
  begin
   if ((A == 3))
     next_state <= s3;
   if ((A == 3))
     next_state <= s4;
   end
  endcase
end

always @ (state)
  begin
  case(state)
    s0: 
    begin
        x = 1;
        y = 0;
    end
    s1: 
    begin
        x = 0;
        y = 1;
    end
    s2: 
    begin
        x = 3;
        y = 1;
    end
  endcase
 end

endmodule