// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VTop.h for the primary calling header

#include "VTop.h"
#include "VTop__Syms.h"

//==========

VL_CTOR_IMP(VTop) {
    VTop__Syms* __restrict vlSymsp = __VlSymsp = new VTop__Syms(this, name());
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Reset internal values
    
    // Reset structure values
    _ctor_var_reset();
}

void VTop::__Vconfigure(VTop__Syms* vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->__VlSymsp = vlSymsp;
    if (false && this->__VlSymsp) {}  // Prevent unused
    Verilated::timeunit(-12);
    Verilated::timeprecision(-12);
}

VTop::~VTop() {
    VL_DO_CLEAR(delete __VlSymsp, __VlSymsp = NULL);
}

void VTop::_initial__TOP__1(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_initial__TOP__1\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->Ext_Ctrl = 0U;
    vlTOPp->Ext_Status = 0U;
    vlTOPp->Ext_TxFIFO = 0U;
    vlTOPp->Ext_RxFIFO = 0U;
}

void VTop::_settle__TOP__3(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_settle__TOP__3\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment = 0U;
    if ((1U != (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
        if ((3U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
            vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment 
                = (0x56U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter));
        }
    }
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment = 0U;
    if ((1U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
        vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment 
            = (0x2bU == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter));
    }
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment = 0U;
    if ((1U != (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
        if ((3U != (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
            if ((2U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
                vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment 
                    = (0x56U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter));
            }
        }
    }
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_next_state 
        = ((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
            ? ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                ? ((8U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter))
                    ? 2U : 3U) : ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment)
                                   ? 0U : 2U)) : ((1U 
                                                   & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                                   ? 
                                                  ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment)
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                                     ? 0U
                                                     : 3U)
                                                    : 1U)
                                                   : 
                                                  ((2U 
                                                    & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                                    ? 0U
                                                    : 1U)));
}

void VTop::_eval_initial(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_eval_initial\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_initial__TOP__1(vlSymsp);
    vlTOPp->__Vclklast__TOP__IO_Clk_I = vlTOPp->IO_Clk_I;
    vlTOPp->__Vclklast__TOP__IO_Rst_I = vlTOPp->IO_Rst_I;
}

void VTop::final() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::final\n"); );
    // Variables
    VTop__Syms* __restrict vlSymsp = this->__VlSymsp;
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
}

void VTop::_eval_settle(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_eval_settle\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_settle__TOP__3(vlSymsp);
    vlTOPp->__Vm_traceActivity[1U] = 1U;
    vlTOPp->__Vm_traceActivity[0U] = 1U;
}

void VTop::_ctor_var_reset() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_ctor_var_reset\n"); );
    // Body
    Ext_RxFIFO = VL_RAND_RESET_I(32);
    Ext_TxFIFO = VL_RAND_RESET_I(32);
    Ext_Status = VL_RAND_RESET_I(32);
    Ext_Ctrl = VL_RAND_RESET_I(32);
    IO_Rst_I = VL_RAND_RESET_I(1);
    IO_Clk_I = VL_RAND_RESET_I(1);
    IO_Rx_I = VL_RAND_RESET_I(1);
    IO_Tx_O = VL_RAND_RESET_I(1);
    Top__DOT__uart_rx_instance__DOT__reg_fsm_state = VL_RAND_RESET_I(2);
    Top__DOT__uart_rx_instance__DOT__reg_fsm_next_state = VL_RAND_RESET_I(2);
    Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment = VL_RAND_RESET_I(1);
    Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment = VL_RAND_RESET_I(1);
    Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment = VL_RAND_RESET_I(1);
    Top__DOT__uart_rx_instance__DOT__reg_clk_counter = VL_RAND_RESET_I(8);
    Top__DOT__uart_rx_instance__DOT__reg_bit_counter = VL_RAND_RESET_I(4);
    Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit = VL_RAND_RESET_I(2);
    Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag = VL_RAND_RESET_I(1);
    Top__DOT__uart_rx_instance__DOT__reg_recv_data = VL_RAND_RESET_I(8);
    { int __Vi0=0; for (; __Vi0<2; ++__Vi0) {
            __Vm_traceActivity[__Vi0] = VL_RAND_RESET_I(1);
    }}
}
