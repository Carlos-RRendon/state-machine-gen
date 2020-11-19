//=============================================================================
// FSM Verilog design
//=============================================================================


//-----------------------------------------------------------------------------
// Module name and ports declaration
//-----------------------------------------------------------------------------

module state_machine (
input clk, rst,
input [5:0] OP,
output reg IorD,
output reg AluSrcA,
output reg [1:0] ALUSrcB,
output reg [1:0] AlUop,
output reg [1:0] PCSrc,
output reg IRWrite,
output reg PCWrite,
output reg RegDst,
output reg MemtoReg,
output reg RegWrite,
output reg Branch,
output reg MemWrite,
output reg PcSrc);

//-----------------------------------------------------------------------------
// FSM states declaration
//-----------------------------------------------------------------------------
parameter s0 = 4'b0;
parameter s1 = 4'b1;
parameter s2 = 4'b10;
parameter s3 = 4'b11;
parameter s4 = 4'b100;
parameter s5 = 4'b101;
parameter s6 = 4'b110;
parameter s7 = 4'b111;
parameter s8 = 4'b1000;
parameter s9 = 4'b1001;
parameter s10 = 4'b1010;
parameter s11 = 4'b1011;
reg [3:0] state, next_state;
 
 //FSM Initialization state
initial begin
  state=0;
end
 
 //FSM State transitions (clock dependant)
always @ (posedge clk or rst)
  begin
  if (rst) state <= s0;
  else state <= next_state;
  end

//-----------------------------------------------------------------------------
// FSM States assignment
//-----------------------------------------------------------------------------
always @ (state, OP)
begin
 case(state)
s0: 
  next_state <= s1;
s1: 
  begin
   if ((OP == 35))
     next_state <= s2;
   if ((OP == 43))
     next_state <= s2;
   if ((OP == 0))
     next_state <= s2;
   if ((OP == 4))
     next_state <= s8;
   if ((OP == 8))
     next_state <= s9;
   if ((OP == 43))
     next_state <= s11;
   end
s2: 
  begin
   if ((OP == 35))
     next_state <= s3;
   if ((OP == 45))
     next_state <= s5;
   end
s3: 
  next_state <= s4;
s4: 
  next_state <= s0;
s5: 
  next_state <= s6;
s6: 
  next_state <= s7;
s7: 
  next_state <= s0;
s8: 
  next_state <= s0;
s9: 
  next_state <= s10;
s10: 
  next_state <= s0;
s11: 
  next_state <= s0;
  endcase
end

//-----------------------------------------------------------------------------
// FSM Outputs assignment
//-----------------------------------------------------------------------------
always @ (state)
  begin
  case(state)
    s0: 
    begin
        IorD = 0;
        AluSrcA = 1;
        ALUSrcB = 1;
        AlUop = 0;
        PCSrc = 0;
        IRWrite = 1;
        PCWrite = 1;
        RegDst = 0;
        MemtoReg = 0;
        RegWrite = 0;
        Branch = 0;
        MemWrite = 0;
    end
    s1: 
    begin
        AluSrcA = 1;
        ALUSrcB = 1;
        AlUop = 0;
        PCSrc = 0;
        IRWrite = 1;
        PCWrite = 1;
    end
    s2: 
    begin
        AluSrcA = 1;
        ALUSrcB = 2;
        AlUop = 0;
    end
    s3: 
    begin
        IorD = 1;
    end
    s4: 
    begin
        RegDst = 0;
        MemtoReg = 1;
        RegWrite = 1;
    end
    s5: 
    begin
        IorD = 1;
        MemWrite = 1;
    end
    s6: 
    begin
        AluSrcA = 1;
        ALUSrcB = 0;
        AlUop = 2;
    end
    s7: 
    begin
        RegDst = 1;
        MemtoReg = 0;
        RegWrite = 1;
    end
    s8: 
    begin
        AluSrcA = 1;
        ALUSrcB = 0;
        AlUop = 1;
        PcSrc = 1;
        Branch = 1;
    end
    s9: 
    begin
        AluSrcA = 1;
        ALUSrcB = 1;
        AlUop = 0;
    end
    s10: 
    begin
        RegDst = 0;
        MemtoReg = 0;
        RegWrite = 1;
    end
    s11: 
    begin
        PCSrc = 2;
        PCWrite = 1;
    end
  endcase
 end

endmodule

//=============================================================================
//=============================================================================

