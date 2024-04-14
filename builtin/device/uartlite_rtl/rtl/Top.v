module Top #(
    parameter           Param_BaurdRate   = 115200,
    parameter           Param_ClkFreq     = 10_000_000,
    parameter           Param_PayloadBits = 8
) (
    inout  wire [31:0]  Ext_RxFIFO,
    inout  wire [31:0]  Ext_TxFIFO,
    inout  wire [31:0]  Ext_Status,
    inout  wire [31:0]  Ext_Ctrl,
    input  wire         IO_Rst_I,           // Reset on negedge, IDLE on neg
    input  wire         IO_Clk_I,
    input  wire         IO_Rx_I,
    output wire         IO_Tx_O
);

wire                        wire_rx_done_flag;
wire[Param_PayloadBits-1:0] wire_rx_data;

// --- Rx Module Instance
/* verilator lint_off PINMISSING */
UartRx #(
    .Param_BaurdRate(Param_BaurdRate),
    .Param_ClkFreq(Param_ClkFreq),
    .Param_PayloadBits(Param_PayloadBits)
) uart_rx_instance(
    .IO_Rst_I(IO_Rst_I),
    .IO_Clk_I(IO_Clk_I),
    .IO_Rx_I(IO_Rx_I),
    .IO_RxDone_O(wire_rx_done_flag),
    .IO_RxData_O(wire_rx_data)
);
/* verilator lint_on PINMISSING */


endmodule