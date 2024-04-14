`define WIDTH 32
module Top(
    // ----- AXI Interface
    // --- Clock & Reset
    input  wire                  ACLK,      // Clock
    input  wire                  ARESETn,   // 
    // --- ADDRESS WRITE CHANNEL
    output wire                  AWREADY,   // Address Write Ready    
    input  reg                   AWVALID,   // Address Write Valid, HIGH means `AWADDR` valid
    input  reg  [`WIDTH-1:0]     AWADDR,    // Address Write Address
    // --- DATA WRITE CHANNEL
    output wire                  WREADY,    // Write Ready
    input  reg                   WVALID,    // Write Valid, HIGH means `WDATA` valid
    input  reg  [(`WIDTH/8)-1:0] WSTRB,     // Indicates which bits are valid in `WDATA`, ignored by some IP cores
    input  reg  [`WIDTH-1:0]     WDATA,     // Write data, data to be writed
    // --- WRITE RESPONSE CHANNEL
    output wire [1:0]            BRESP,     // Bus Response, feedback of data write
    output wire                  BVALID,    // Bus Valid
    input  reg                   BREADY,    // Bus Ready, HIGH means ready to receive write feedback
    // --- READ ADDRESS CHANNEL
    output wire                  ARREADY,   // Address Read Ready, HIGH means master ready to receive read address
    input  reg                   ARVALID,   // Address Read Valid, HIGH means `ARADDR` is valid
    input  reg  [`WIDTH-1:0]     ARADDR,    // Address Read Address, address to read from slave
    // --- READ DATA CHANNEL
    output wire [`WIDTH-1:0]     RDATA,     // Read Data, data read from slave
    output wire [1:0]            RRESP,     // Read Response, feedback of data read
    output wire                  RVALID,    // Read Valid, HIGH means `RDATA` valid
    input  reg                   RREADY,    // Read Ready, HIGH means master read ready ?
);


// Please instantiate your rtl module here


endmodule