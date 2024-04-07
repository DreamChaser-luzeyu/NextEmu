`define WIDTH 32
module AXIMaster(
    input  wire                  ACLK,      // Clock
    input  wire                  ARESETn,   // 
    // --- ADDRESS WRITE CHANNEL
    input  wire                  AWREADY,   // Address Write Ready    
    output reg                   AWVALID,   // Address Write Valid, HIGH means `AWADDR` valid
    output reg  [`WIDTH-1:0]     AWADDR,    // Address Write Address
    // --- DATA WRITE CHANNEL
    input  wire                  WREADY,    // Write Ready
    output reg                   WVALID,    // Write Valid, HIGH means `WDATA` valid
    output reg  [(`WIDTH/8)-1:0] WSTRB,     // Indicates which bits are valid in `WDATA`
    output reg  [`WIDTH-1:0]     WDATA,     // Write data, data to be writed
    // --- WRITE RESPONSE CHANNEL
    input  wire [1:0]            BRESP,     // Bus Response, feedback of data write
    input  wire                  BVALID,    // Bus Valid
    output reg                   BREADY,    // Bus Ready, HIGH means ready to receive write feedback
    // --- READ ADDRESS CHANNEL
    input  wire                  ARREADY,   // Address Read Ready, HIGH means master ready to receive read address
    output reg                   ARVALID,   // Address Read Valid, HIGH means `ARADDR` is valid
    output reg  [`WIDTH-1:0]     ARADDR,    // Address Read Address, address to read from slave
    // --- READ DATA CHANNEL
    input  wire [`WIDTH-1:0]     RDATA,     // Read Data, data read from slave
    input  wire [1:0]            RRESP,     // Read Response, feedback of data read
    input  wire                  RVALID,    // Read Valid, HIGH means `RDATA` valid
    output reg                   RREADY,    // Read Ready, HIGH means master ready to receive slave feedback
    // --- Ports to be connected to CPU
    input  wire [`WIDTH-1:0]     awaddr,    
    input  wire [(`WIDTH/8)-1:0] wstrb,
    input  wire [`WIDTH-1:0]     wdata,
    input  wire [`WIDTH-1:0]     araddr,
    output reg  [`WIDTH-1:0]     data_out,
    output wire [1:0]            feedback_r,
    output wire [1:0]            feedback_w
);

// ----- Feedback
assign feedback_r = RRESP;
assign feedback_w = BRESP;

// ----- WRITE ADDRESS CHANNEL MASTER -----
parameter [1:0] WA_IDLE_M  = 2'b00,
                WA_VALID_M = 2'b01,
                WA_ADDR_M  = 2'b10,
                WA_WAIT_M  = 2'b11;
// ----- FSM 
reg [1:0] WAState_M, WANext_state_M;
// --- FSM Initialize & State Transition Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn) WAState_M <= WA_IDLE_M;
    else         WAState_M <= WANext_state_M;
end
// --- FSM Next State Logic
always @(*) begin
    case(WAState_M)
         WA_IDLE_M:  if(awaddr > 32'h0) WANext_state_M = WA_VALID_M;
                     else               WANext_state_M = WA_IDLE_M;
         WA_VALID_M: if(AWREADY)        WANext_state_M = WA_ADDR_M;
                     else               WANext_state_M = WA_VALID_M;
         WA_ADDR_M:                     WANext_state_M = WA_WAIT_M;
         WA_WAIT_M:  if(BVALID)         WANext_state_M = WA_IDLE_M;
                     else               WANext_state_M = WA_WAIT_M;
         default:                       WANext_state_M = WA_IDLE_M;
    endcase // WAState_M
end
// ----- Output Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn)       AWVALID <= 1'b0;
    else begin
        case (WANext_state_M)
            WA_IDLE_M: AWVALID <= 1'b0;
            WA_VALID_M: begin
                AWVALID <= 1'b1;
                AWADDR  <= awaddr;
            end
            WA_ADDR_M: AWVALID <= 1'b0;
            WA_WAIT_M: AWVALID <= 1'b0;
            default:   AWVALID <= 1'b0;
        endcase
    end
end


// ----- WRITE DATA CHANNEL MASTER
parameter [1:0] W_IDLE_M  = 2'b00,
                W_GET_M   = 2'b01,
                W_WAIT_M  = 2'b10,
                W_TRANS_M = 2'b11;
// ----- FSM
reg [1:0] WState_M,WNext_state_M;
// --- FSM Initialize & State Transition Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn) WState_M <= W_IDLE_M;
    else         WState_M <= WNext_state_M;
end
// --- FSM Next State Logic
always @(*) begin
    case(WState_M)
        W_IDLE_M: WNext_state_M = W_GET_M;
        W_GET_M: begin
            if(AWREADY) WNext_state_M = W_WAIT_M;
            else        WNext_state_M = W_GET_M;
        end
        W_WAIT_M: begin
            if(WREADY)  WNext_state_M = W_TRANS_M;
            else        WNext_state_M = W_WAIT_M;
        end
        W_TRANS_M: WNext_state_M = W_IDLE_M;
        default:   WNext_state_M = W_IDLE_M;
    endcase // WNext_state_M
end
// ----- Output Logic
always@(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn)         WVALID <=1'B0;
    else begin
        case(WNext_state_M)
            W_IDLE_M:    WVALID <= 1'B0;
            W_GET_M: begin
                WVALID <= 1'B0;
                WSTRB  <= wstrb;
                WDATA  <= wdata;
            end
            W_WAIT_M:    WVALID <= 1'B1;
            W_TRANS_M:   WVALID <= 1'B0;
            default:     WVALID <= 1'B0;
        endcase // WNext_state_M
    end
end

// ----- WRITE RESPONSE CHANNEL MASTER
parameter [1:0] B_IDLE_M = 2'b00,
                B_START_M= 2'b01,
                B_READY_M= 2'b10;
// ----- FSM
reg [1:0] BState_M,BNext_state_M;
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn) BState_M <= B_IDLE_M;
    else         BState_M <= BNext_state_M;
end
always @(*) begin
    case(BState_M)
        B_IDLE_M:  begin 
			if(AWREADY) BNext_state_M = B_START_M;
            else        BNext_state_M = B_IDLE_M; 
        end
        B_START_M: begin 
            if(BVALID)  BNext_state_M = B_READY_M;
            else        BNext_state_M = B_START_M; 
        end
        B_READY_M: BNext_state_M = B_IDLE_M;
        default:   BNext_state_M = B_IDLE_M;
    endcase // BState_M
end
// ----- Output Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn)        BREADY <= 1'B0;
    else begin
        case(BNext_state_M)
            B_IDLE_M:   BREADY <= 1'B0;
            B_START_M:  BREADY <= 1'B1;
            B_READY_M:  BREADY <= 1'B0;
            default:    BREADY <= 1'B0;
        endcase // BNext_state_M
    end
end

// ----- READ ADDR CHANNEL MASTER
parameter [1:0] AR_IDLE_M  = 2'B00,
                AR_VALID_M = 2'B01;
// ----- FSM
reg [1:0] ARState_M,ARNext_state_M;
always@(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn) ARState_M <= AR_IDLE_M;
    else         ARState_M <= ARNext_state_M;
end
always @(*) begin
    case(ARState_M)
        AR_IDLE_M:  begin 
			if(araddr > 32'h0) ARNext_state_M = AR_VALID_M;
            else               ARNext_state_M = AR_IDLE_M; 
        end
        AR_VALID_M: begin 
			if(ARREADY)        ARNext_state_M = AR_IDLE_M;
            else               ARNext_state_M = AR_VALID_M; 
        end
        default:               ARNext_state_M = AR_IDLE_M;
    endcase // ARState_M
end
// ----- Output Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn)  ARVALID <= 1'B0;
    else
        case(ARNext_state_M)
            AR_IDLE_M:  ARVALID <= 1'B0;
            AR_VALID_M: begin   
			    ARVALID <= 1'B1;                                 
                ARADDR  <= araddr; 
            end
            default:    ARVALID <= 1'B0;
        endcase // ARNext_state_M
end

// ----- READ DATA CHANNEL MASTER
parameter [1:0] R_IDLE_M  = 2'B00,
                R_READY_M = 2'B01;
                //R_TRAN_M  = 2'B10;
// ----- FSM
reg   [1:0] RState_M, RNext_state_M;
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn)    RState_M <= R_IDLE_M;
    else            RState_M <= RNext_state_M;
end
always @(*) begin
    case(RState_M)
        R_IDLE_M : begin
            if(ARREADY && araddr != awaddr) RNext_state_M = R_READY_M;
            else begin
                RNext_state_M = R_IDLE_M;
                $display("CANNOT READ WRITE AT SAME LOCATION %t",$time);
            end
        end
        R_READY_M: begin
            if(RVALID)  RNext_state_M = R_IDLE_M;
            else RNext_state_M = R_READY_M; end
        // R_TRAN_M: RNext_state_M = R_IDLE_M;
        default: RNext_state_M = R_IDLE_M;
    endcase // RState_M
end
// ----- Output Logic
always @(posedge ACLK or negedge ARESETn) begin
    if(!ARESETn) RREADY <= 1'B0;
    else begin
        case(RNext_state_M)
            R_IDLE_M: RREADY <= 1'B0;
            R_READY_M: begin
                RREADY   <= 1'B1;
                data_out <= RDATA;
            end
            // R_TRAN_M: begin  RREADY <= 1'B0;
            //                data_out <= RDATA; end
            default : RREADY <=  1'B0;
        endcase // RNext_state_M
    end
end

endmodule
