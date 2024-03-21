// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Primary design header
//
// This header should be included by all source files instantiating the design.
// The class here is then constructed to instantiate the design.
// See the Verilator manual for examples.

#ifndef _VUARTRX_H_
#define _VUARTRX_H_  // guard

#include "verilated_heavy.h"

//==========

class VUartRx__Syms;
class VUartRx_VerilatedVcd;


//----------

VL_MODULE(VUartRx) {
  public:
    
    // PORTS
    // The application code writes and reads these signals to
    // propagate new values into/out from the Verilated model.
    VL_IN8(IO_Rst_I,0,0);
    VL_IN8(IO_Clk_I,0,0);
    VL_IN8(IO_Rx_I,0,0);
    VL_OUT8(IO_RxDone_O,0,0);
    VL_OUT8(IO_RxData_O,7,0);
    VL_OUT8(IO_Debug_Sample_O,0,0);
    
    // LOCAL SIGNALS
    // Internals; generally not touched by application code
    CData/*1:0*/ UartRx__DOT__reg_fsm_state;
    CData/*1:0*/ UartRx__DOT__reg_fsm_next_state;
    CData/*0:0*/ UartRx__DOT__reg_data_bit_mid_moment;
    CData/*0:0*/ UartRx__DOT__reg_start_bit_mid_moment;
    CData/*0:0*/ UartRx__DOT__reg_stop_bit_mid_moment;
    CData/*7:0*/ UartRx__DOT__reg_clk_counter;
    CData/*3:0*/ UartRx__DOT__reg_bit_counter;
    CData/*1:0*/ UartRx__DOT__reg_recv_data_bit;
    CData/*0:0*/ UartRx__DOT__reg_rx_done_flag;
    CData/*7:0*/ UartRx__DOT__reg_recv_data;
    
    // LOCAL VARIABLES
    // Internals; generally not touched by application code
    CData/*0:0*/ __Vclklast__TOP__IO_Clk_I;
    CData/*0:0*/ __Vclklast__TOP__IO_Rst_I;
    CData/*0:0*/ __Vm_traceActivity[2];
    
    // INTERNAL VARIABLES
    // Internals; generally not touched by application code
    VUartRx__Syms* __VlSymsp;  // Symbol table
    
    // CONSTRUCTORS
  private:
    VL_UNCOPYABLE(VUartRx);  ///< Copying not allowed
  public:
    /// Construct the model; called by application code
    /// The special name  may be used to make a wrapper with a
    /// single model invisible with respect to DPI scope names.
    VUartRx(const char* name = "TOP");
    /// Destroy the model; called (often implicitly) by application code
    ~VUartRx();
    /// Trace signals in the model; called by application code
    void trace(VerilatedVcdC* tfp, int levels, int options = 0);
    
    // API METHODS
    /// Evaluate the model.  Application must call when inputs change.
    void eval() { eval_step(); }
    /// Evaluate when calling multiple units/models per time step.
    void eval_step();
    /// Evaluate at end of a timestep for tracing, when using eval_step().
    /// Application must call after all eval() and before time changes.
    void eval_end_step() {}
    /// Simulation complete, run final blocks.  Application must call on completion.
    void final();
    
    // INTERNAL METHODS
  private:
    static void _eval_initial_loop(VUartRx__Syms* __restrict vlSymsp);
  public:
    void __Vconfigure(VUartRx__Syms* symsp, bool first);
  private:
    static QData _change_request(VUartRx__Syms* __restrict vlSymsp);
    static QData _change_request_1(VUartRx__Syms* __restrict vlSymsp);
    void _ctor_var_reset() VL_ATTR_COLD;
  public:
    static void _eval(VUartRx__Syms* __restrict vlSymsp);
  private:
#ifdef VL_DEBUG
    void _eval_debug_assertions();
#endif  // VL_DEBUG
  public:
    static void _eval_initial(VUartRx__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _eval_settle(VUartRx__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _sequent__TOP__1(VUartRx__Syms* __restrict vlSymsp);
    static void _settle__TOP__2(VUartRx__Syms* __restrict vlSymsp) VL_ATTR_COLD;
  private:
    static void traceChgSub0(void* userp, VerilatedVcd* tracep);
    static void traceChgTop0(void* userp, VerilatedVcd* tracep);
    static void traceCleanup(void* userp, VerilatedVcd* /*unused*/);
    static void traceFullSub0(void* userp, VerilatedVcd* tracep) VL_ATTR_COLD;
    static void traceFullTop0(void* userp, VerilatedVcd* tracep) VL_ATTR_COLD;
    static void traceInitSub0(void* userp, VerilatedVcd* tracep) VL_ATTR_COLD;
    static void traceInitTop(void* userp, VerilatedVcd* tracep) VL_ATTR_COLD;
    void traceRegister(VerilatedVcd* tracep) VL_ATTR_COLD;
    static void traceInit(void* userp, VerilatedVcd* tracep, uint32_t code) VL_ATTR_COLD;
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

//----------


#endif  // guard
