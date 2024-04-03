module UartRx #(
    parameter                           Param_BaurdRate   = 9600,
//    parameter                           Param_ClkFreq     = 10_000_000,
    parameter                           Param_ClkFreq     = 1000000,
    parameter                           Param_PayloadBits = 8
)(
    input  wire                         IO_Rst_I,
    input  wire                         IO_Clk_I,
    input  wire                         IO_Rx_I,
    output wire                         IO_RxDone_O,
    output wire[Param_PayloadBits-1:0]  IO_RxData_O,
    output wire                         IO_Debug_Sample_O
);

// Note: Has to add additional 1, to make sure counter bigger than threshole possible
localparam Param_ClkCounterBitWidth = $clog2(Param_ClkFreq / Param_BaurdRate) + 1;
localparam Param_BitCounterBitWidth = $clog2(Param_PayloadBits) + 1;
localparam clk_per_bit = (Param_ClkFreq / Param_BaurdRate);
localparam[Param_ClkCounterBitWidth-1:0] Param_ClkPerBit = clk_per_bit[Param_ClkCounterBitWidth-1:0];


// ----- Register definitions
// --- FSM State Definitions
localparam Param_StateIDLE     = 2'b00;
localparam Param_StateStartBit = 2'b01;
localparam Param_StateDataBits = 2'b11;
localparam Param_StateStopBit  = 2'b10;
reg[1:0]   reg_fsm_state;             // Current FSM State
reg[1:0]   reg_fsm_next_state;        // Next FSM State
// --- Moment flags
reg reg_data_bit_mid_moment;          // whether we are at the middle of data bit
reg reg_start_bit_mid_moment;         // whether we are at the middle of start bit
reg reg_stop_bit_mid_moment;
assign IO_Debug_Sample_O = reg_data_bit_mid_moment | reg_start_bit_mid_moment | reg_stop_bit_mid_moment;
// --- Clock Counters
reg[Param_ClkCounterBitWidth-1:0]   reg_clk_counter;            // Counts the index of the bit we are receiving
reg[Param_BitCounterBitWidth-1:0]   reg_bit_counter;            // Counts which bit we are receiving
// ---
reg[1:0] reg_recv_data_bit;
reg reg_rx_done_flag;
reg[Param_PayloadBits-1:0] reg_recv_data;

assign IO_RxData_O = reg_recv_data;
assign IO_RxDone_O = reg_rx_done_flag;

// ----- Data clock domain synchronize logic
always @(posedge IO_Clk_I or negedge IO_Rst_I) begin
    if(~IO_Rst_I) begin
        reg_recv_data_bit <= 2'b00;                            // Reset with data 0
    end else begin
        reg_recv_data_bit <= {reg_recv_data_bit[0], IO_Rx_I};  // Latch rx bit
    end
end

// ----- Moments
// --- Moment logic
always @(*) begin
    reg_start_bit_mid_moment = 1'b0;
    reg_data_bit_mid_moment = 1'b0;
    reg_stop_bit_mid_moment = 1'b0;   // Initialize all flags with 0
    /* verilator lint_off CASEINCOMPLETE */
    case (reg_fsm_state)
    Param_StateStartBit: reg_start_bit_mid_moment = (reg_clk_counter == Param_ClkPerBit / 2);
                                      // sample at cycle duration mid
    Param_StateDataBits: reg_data_bit_mid_moment = (reg_clk_counter == Param_ClkPerBit);
                                      // Need to delay another FULL CYCLE DURATION to reach next cycle duration mid
                                      // So no need to divide by 2
    Param_StateStopBit:  reg_stop_bit_mid_moment = (reg_clk_counter == Param_ClkPerBit);
    endcase
    /* verilator lint_on CASEINCOMPLETE */
end

// ----- Counter Logic
always @(posedge IO_Clk_I or negedge IO_Rst_I) begin
    if(~IO_Rst_I) begin
        reg_clk_counter <= {Param_ClkCounterBitWidth{1'b0}};   // Clear counters on reset
        reg_bit_counter <= {Param_BitCounterBitWidth{1'b0}};
    end else begin
        case(reg_fsm_state)
        Param_StateIDLE: begin
            reg_clk_counter <= {Param_ClkCounterBitWidth{1'b0}};
            reg_bit_counter <= {Param_BitCounterBitWidth{1'b0}};  // Disable counters on IDLE
        end
        Param_StateStartBit: begin
            if(reg_clk_counter < Param_ClkPerBit / 2) reg_clk_counter <= (reg_clk_counter + 1'b1);
            else reg_clk_counter <= {Param_ClkCounterBitWidth{1'b0}};
        end
        Param_StateDataBits: begin
            // --- Clock Counter
            if(reg_clk_counter < Param_ClkPerBit) reg_clk_counter <= (reg_clk_counter + 1);
            else reg_clk_counter <= {Param_ClkCounterBitWidth{1'b0}};
            // --- Data Bits Counter
            if(reg_data_bit_mid_moment == 1'b1) begin
                // - Check if received all bits
                if(reg_bit_counter < Param_PayloadBits) reg_bit_counter <= (reg_bit_counter + 1'b1);
                else reg_bit_counter <= {Param_BitCounterBitWidth{1'b0}};
            end
        end
        Param_StateStopBit: begin
            if(reg_clk_counter < Param_ClkPerBit) reg_clk_counter <= (reg_clk_counter + 1'b1);
            else reg_clk_counter <= {Param_ClkCounterBitWidth{1'b0}};
        end
        endcase
    end
end

// ----- RX Done Logic
always @(posedge IO_Clk_I or negedge IO_Rst_I) begin
    if(!IO_Rst_I) reg_rx_done_flag <= 1'b0;             // Init on reset
    else begin
        reg_rx_done_flag <= reg_stop_bit_mid_moment;   // rx done on reaching stop bit mid
//        if(reg_rx_done_flag) $display("UartRTL: Parsed 1 byte %h", IO_RxData_O);
    end
end

// ----- Grab Data Logic
always @(posedge IO_Clk_I or negedge IO_Rst_I) begin
    if(~IO_Rst_I) begin
        reg_recv_data <= {Param_PayloadBits{1'b0}};              // Flush recv data
    end else if(reg_data_bit_mid_moment == 1'b1) begin
        reg_recv_data[reg_bit_counter[Param_BitCounterBitWidth-2:0]] <= reg_recv_data_bit[1];  // Latch data bit
        // reg_bit_counter is 1 bit wider than index width   ^^^   of reg_recv_data so need to truncate 1
    end
end

// ----- FSM
// --- FSM Transit Logic
always @(posedge IO_Clk_I or negedge IO_Rst_I) begin
    if(~IO_Rst_I) begin
        reg_fsm_state <= Param_StateIDLE;    // Reset to IDLE state
    end else begin
        reg_fsm_state <= reg_fsm_next_state; // Move to next state
    end
end
// --- Next State Logic
always @(*) begin
    reg_fsm_next_state = Param_StateIDLE;    // Init next state
    case(reg_fsm_state)
    Param_StateIDLE: begin
        if(reg_recv_data_bit[1] == 1'b0) reg_fsm_next_state = Param_StateStartBit;
        else reg_fsm_next_state = Param_StateIDLE;
    end
    Param_StateStartBit: begin
        if(reg_start_bit_mid_moment == 1'b1) begin       // Time to grab start bit
            if(reg_recv_data_bit[1] == 1'b0) reg_fsm_next_state = Param_StateDataBits;
                                                         // Check if receive bit still 1'b0
            else reg_fsm_next_state = Param_StateIDLE;   // Treat as noise, go back to IDLE
        end
        else reg_fsm_next_state = Param_StateStartBit;   // Not time to grab start bit, keep current state
    end
    Param_StateDataBits: begin
        if(reg_bit_counter == Param_PayloadBits) reg_fsm_next_state = Param_StateStopBit;
                                                         // Check if finish receiving all bits
        else reg_fsm_next_state = Param_StateDataBits;   // Not finish receiving, keep current state
    end
    Param_StateStopBit: begin
        if(reg_stop_bit_mid_moment == 1'b1) reg_fsm_next_state = Param_StateIDLE;
        else reg_fsm_next_state = Param_StateStopBit;
    end
    endcase
end


endmodule