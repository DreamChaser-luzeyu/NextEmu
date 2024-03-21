// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VUartRx.h for the primary calling header

#include "VUartRx.h"
#include "VUartRx__Syms.h"

//==========

void VUartRx::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate VUartRx::eval\n"); );
    VUartRx__Syms* __restrict vlSymsp = this->__VlSymsp;  // Setup global symbol table
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
#ifdef VL_DEBUG
    // Debug assertions
    _eval_debug_assertions();
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        vlSymsp->__Vm_activity = true;
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("/media/luzeyu/Files/Study/NextEmu/builtin/device/uart_rtl/rtl/UartRx.v", 1, "",
                "Verilated model didn't converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

void VUartRx::_eval_initial_loop(VUartRx__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    _eval_initial(vlSymsp);
    vlSymsp->__Vm_activity = true;
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        _eval_settle(vlSymsp);
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("/media/luzeyu/Files/Study/NextEmu/builtin/device/uart_rtl/rtl/UartRx.v", 1, "",
                "Verilated model didn't DC converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

VL_INLINE_OPT void VUartRx::_sequent__TOP__1(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_sequent__TOP__1\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    CData/*1:0*/ __Vdly__UartRx__DOT__reg_recv_data_bit;
    // Body
    __Vdly__UartRx__DOT__reg_recv_data_bit = vlTOPp->UartRx__DOT__reg_recv_data_bit;
    __Vdly__UartRx__DOT__reg_recv_data_bit = ((IData)(vlTOPp->IO_Rst_I)
                                               ? ((2U 
                                                   & ((IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit) 
                                                      << 1U)) 
                                                  | (IData)(vlTOPp->IO_Rx_I))
                                               : 0U);
    if (vlTOPp->IO_Rst_I) {
        if (vlTOPp->UartRx__DOT__reg_data_bit_mid_moment) {
            vlTOPp->UartRx__DOT__reg_recv_data = ((
                                                   (~ 
                                                    ((IData)(1U) 
                                                     << 
                                                     (7U 
                                                      & (IData)(vlTOPp->UartRx__DOT__reg_bit_counter)))) 
                                                   & (IData)(vlTOPp->UartRx__DOT__reg_recv_data)) 
                                                  | ((1U 
                                                      & ((IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit) 
                                                         >> 1U)) 
                                                     << 
                                                     (7U 
                                                      & (IData)(vlTOPp->UartRx__DOT__reg_bit_counter))));
        }
    } else {
        vlTOPp->UartRx__DOT__reg_recv_data = 0U;
    }
    if (vlTOPp->IO_Rst_I) {
        if (VL_UNLIKELY(vlTOPp->UartRx__DOT__reg_rx_done_flag)) {
            VL_WRITEF("UartRTL: Parsed 1 byte %x\n",
                      8,vlTOPp->IO_RxData_O);
        }
        vlTOPp->UartRx__DOT__reg_rx_done_flag = vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment;
    } else {
        vlTOPp->UartRx__DOT__reg_rx_done_flag = 0U;
    }
    if (vlTOPp->IO_Rst_I) {
        if ((2U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
            if ((1U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))) {
                if (vlTOPp->UartRx__DOT__reg_data_bit_mid_moment) {
                    vlTOPp->UartRx__DOT__reg_bit_counter 
                        = ((8U > (IData)(vlTOPp->UartRx__DOT__reg_bit_counter))
                            ? (0xfU & ((IData)(1U) 
                                       + (IData)(vlTOPp->UartRx__DOT__reg_bit_counter)))
                            : 0U);
                }
            }
        } else {
            if ((1U & (~ (IData)(vlTOPp->UartRx__DOT__reg_fsm_state)))) {
                vlTOPp->UartRx__DOT__reg_bit_counter = 0U;
            }
        }
    } else {
        vlTOPp->UartRx__DOT__reg_bit_counter = 0U;
    }
    vlTOPp->UartRx__DOT__reg_clk_counter = ((IData)(vlTOPp->IO_Rst_I)
                                             ? (0xffU 
                                                & ((2U 
                                                    & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                    ? 
                                                   ((1U 
                                                     & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                     ? 
                                                    ((0x68U 
                                                      > (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      ? 
                                                     ((IData)(1U) 
                                                      + (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      : 0U)
                                                     : 
                                                    ((0x68U 
                                                      > (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      ? 
                                                     ((IData)(1U) 
                                                      + (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      : 0U))
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                                     ? 
                                                    ((0x34U 
                                                      > (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      ? 
                                                     ((IData)(1U) 
                                                      + (IData)(vlTOPp->UartRx__DOT__reg_clk_counter))
                                                      : 0U)
                                                     : 0U)))
                                             : 0U);
    vlTOPp->UartRx__DOT__reg_recv_data_bit = __Vdly__UartRx__DOT__reg_recv_data_bit;
    vlTOPp->IO_RxData_O = vlTOPp->UartRx__DOT__reg_recv_data;
    vlTOPp->IO_RxDone_O = vlTOPp->UartRx__DOT__reg_rx_done_flag;
    vlTOPp->UartRx__DOT__reg_fsm_state = ((IData)(vlTOPp->IO_Rst_I)
                                           ? (IData)(vlTOPp->UartRx__DOT__reg_fsm_next_state)
                                           : 0U);
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

void VUartRx::_eval(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_eval\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if ((((IData)(vlTOPp->IO_Clk_I) & (~ (IData)(vlTOPp->__Vclklast__TOP__IO_Clk_I))) 
         | ((~ (IData)(vlTOPp->IO_Rst_I)) & (IData)(vlTOPp->__Vclklast__TOP__IO_Rst_I)))) {
        vlTOPp->_sequent__TOP__1(vlSymsp);
        vlTOPp->__Vm_traceActivity[1U] = 1U;
    }
    // Final
    vlTOPp->__Vclklast__TOP__IO_Clk_I = vlTOPp->IO_Clk_I;
    vlTOPp->__Vclklast__TOP__IO_Rst_I = vlTOPp->IO_Rst_I;
}

VL_INLINE_OPT QData VUartRx::_change_request(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_change_request\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    return (vlTOPp->_change_request_1(vlSymsp));
}

VL_INLINE_OPT QData VUartRx::_change_request_1(VUartRx__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_change_request_1\n"); );
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    // Change detection
    QData __req = false;  // Logically a bool
    return __req;
}

#ifdef VL_DEBUG
void VUartRx::_eval_debug_assertions() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VUartRx::_eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((IO_Rst_I & 0xfeU))) {
        Verilated::overWidthError("IO_Rst_I");}
    if (VL_UNLIKELY((IO_Clk_I & 0xfeU))) {
        Verilated::overWidthError("IO_Clk_I");}
    if (VL_UNLIKELY((IO_Rx_I & 0xfeU))) {
        Verilated::overWidthError("IO_Rx_I");}
}
#endif  // VL_DEBUG
