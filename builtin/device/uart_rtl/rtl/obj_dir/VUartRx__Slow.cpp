// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VUartRx.h for the primary calling header

#include "VUartRx.h"
#include "VUartRx__Syms.h"

//==========

VL_CTOR_IMP(VUartRx) {
    VUartRx__Syms* __restrict vlSymsp = __VlSymsp = new VUartRx__Syms(this, name());
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Reset internal values
    
    // Reset structure values
    _ctor_var_reset();
}

void VUartRx::__Vconfigure(VUartRx__Syms* vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->__VlSymsp = vlSymsp;
    if (false && this->__VlSymsp) {}  // Prevent unused
    Verilated::timeunit(-12);
    Verilated::timeprecision(-12);
}

VUartRx::~VUartRx() {
    VL_DO_CLEAR(delete __VlSymsp, __VlSymsp = NULL);
}

void VUartRx::_settle__TOP__2(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_settle__TOP__2\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->IO_RxData_O = vlTOPp->UartRx__DOT__reg_recv_data;
    vlTOPp->IO_RxDone_O = vlTOPp->UartRx__DOT__reg_rx_done_flag;
    vlTOPp->UartRx__DOT__reg_data_bit_mid_moment = 0U;
    if ((1U != (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
        if ((3U == (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
            vlTOPp->UartRx__DOT__reg_data_bit_mid_moment 
                = (0x68U == (IData)(vlTOPp->UartRx__DOT__reg_clk_counter));
        }
    }
    vlTOPp->UartRx__DOT__reg_start_bit_mid_moment = 0U;
    if ((1U == (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
        vlTOPp->UartRx__DOT__reg_start_bit_mid_moment 
            = (0x34U == (IData)(vlTOPp->UartRx__DOT__reg_clk_counter));
    }
    vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment = 0U;
    if ((1U != (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
        if ((3U != (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
            if ((2U == (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
                vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment 
                    = (0x68U == (IData)(vlTOPp->UartRx__DOT__reg_clk_counter));
            }
        }
    }
    vlTOPp->IO_Debug_Sample_O = (((IData)(vlTOPp->UartRx__DOT__reg_data_bit_mid_moment) 
                                  | (IData)(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment)) 
                                 | (IData)(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment));
    vlTOPp->UartRx__DOT__reg_fsm_next_state = ((2U 
                                                & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                ? (
                                                   (1U 
                                                    & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                    ? 
                                                   ((8U 
                                                     == (IData)(vlTOPp->UartRx__DOT__reg_bit_counter))
                                                     ? 2U
                                                     : 3U)
                                                    : 
                                                   ((IData)(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment)
                                                     ? 0U
                                                     : 2U))
                                                : (
                                                   (1U 
                                                    & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                    ? 
                                                   ((IData)(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment)
                                                     ? 
                                                    ((2U 
                                                      & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                                      ? 0U
                                                      : 3U)
                                                     : 1U)
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                                     ? 0U
                                                     : 1U)));
}

void VUartRx::_eval_initial(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_eval_initial\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->__Vclklast__TOP__IO_Clk_I = vlTOPp->IO_Clk_I;
    vlTOPp->__Vclklast__TOP__IO_Rst_I = vlTOPp->IO_Rst_I;
}

void VUartRx::final() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::final\n"); );
    // Variables
    VUartRx__Syms* __restrict vlSymsp = this->__VlSymsp;
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
}

void VUartRx::_eval_settle(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_eval_settle\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_settle__TOP__2(vlSymsp);
    vlTOPp->__Vm_traceActivity[1U] = 1U;
    vlTOPp->__Vm_traceActivity[0U] = 1U;
}

void VUartRx::_ctor_var_reset() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_ctor_var_reset\n"); );
    // Body
    IO_Rst_I = VL_RAND_RESET_I(1);
    IO_Clk_I = VL_RAND_RESET_I(1);
    IO_Rx_I = VL_RAND_RESET_I(1);
    IO_RxDone_O = VL_RAND_RESET_I(1);
    IO_RxData_O = VL_RAND_RESET_I(8);
    IO_Debug_Sample_O = VL_RAND_RESET_I(1);
    UartRx__DOT__reg_fsm_state = VL_RAND_RESET_I(2);
    UartRx__DOT__reg_fsm_next_state = VL_RAND_RESET_I(2);
    UartRx__DOT__reg_data_bit_mid_moment = VL_RAND_RESET_I(1);
    UartRx__DOT__reg_start_bit_mid_moment = VL_RAND_RESET_I(1);
    UartRx__DOT__reg_stop_bit_mid_moment = VL_RAND_RESET_I(1);
    UartRx__DOT__reg_clk_counter = VL_RAND_RESET_I(8);
    UartRx__DOT__reg_bit_counter = VL_RAND_RESET_I(4);
    UartRx__DOT__reg_recv_data_bit = VL_RAND_RESET_I(2);
    UartRx__DOT__reg_rx_done_flag = VL_RAND_RESET_I(1);
    UartRx__DOT__reg_recv_data = VL_RAND_RESET_I(8);
    { int __Vi0=0; for (; __Vi0<2; ++__Vi0) {
            __Vm_traceActivity[__Vi0] = VL_RAND_RESET_I(1);
    }}
}
