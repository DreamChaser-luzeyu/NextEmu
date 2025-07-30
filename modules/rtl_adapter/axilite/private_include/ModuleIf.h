#include "AXILiteAdapter.h"

//class Base_ns::AXILiteAdapter::ModuleIf {
//public:
//    explicit ModuleIf(int bit_width);
//    ~ModuleIf();
//
//    // Note: `_i`, `_o` means input/output (for the adapter),
//    // `_i` means the RTL module actively modify the value, and for this adapter the signal is an input
//    //      so we should ONLY READ the value and never write
//    // `_o` means this adapter actively modify the value, and for this adapter the signal is an output
//    //      so we should ONLY WRITE the value
//    // --- Clk & Rst
//    Interface_ns::WireSignal *axi_ACLK_o;
//    Interface_ns::WireSignal *axi_ARESET_o;
//    // --- Address Write Channel
//    Interface_ns::WireSignal *axi_AWREADY_i;
//    Interface_ns::WireSignal *axi_AWADDR_o;
//    Interface_ns::WireSignal *axi_AWVALID_o;
//    // --- Data Write Channel
//    Interface_ns::WireSignal *axi_WREADY_i;
//    Interface_ns::WireSignal *axi_WDATA_o;
//    Interface_ns::WireSignal *axi_WVALID_o;
//    Interface_ns::WireSignal *axi_WSTRB_o;
//    // --- Write Response Channel
//    Interface_ns::WireSignal *axi_BRESP_i;
//    Interface_ns::WireSignal *axi_BVALID_i;
//    Interface_ns::WireSignal *axi_BREADY_o;
//    // --- Read Address Channel
//    Interface_ns::WireSignal *axi_ARREADY_i;
//    Interface_ns::WireSignal *axi_ARADDR_o;
//    Interface_ns::WireSignal *axi_ARVALID_o;
//    // --- Read Data Channel
//    Interface_ns::WireSignal *axi_RVALID_i;
//    Interface_ns::WireSignal *axi_RDATA_i;
//    Interface_ns::WireSignal *axi_RRESP_i;
//    Interface_ns::WireSignal *axi_RREADY_o;
//};





