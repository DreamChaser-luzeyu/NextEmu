// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VTop.h for the primary calling header

#include "VTop.h"
#include "VTop__Syms.h"

//==========

void VTop::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate VTop::eval\n"); );
    VTop__Syms* __restrict vlSymsp = this->__VlSymsp;  // Setup global symbol table
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
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
            VL_FATAL_MT("/media/luzeyu/Files/Study/NextEmu/builtin/device/uart_rtl/rtl/Top.v", 1, "",
                "Verilated model didn't converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

void VTop::_eval_initial_loop(VTop__Syms* __restrict vlSymsp) {
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
            VL_FATAL_MT("/media/luzeyu/Files/Study/NextEmu/builtin/device/uart_rtl/rtl/Top.v", 1, "",
                "Verilated model didn't DC converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

VL_INLINE_OPT void VTop::_sequent__TOP__2(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_sequent__TOP__2\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    CData/*1:0*/ __Vdly__Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit;
    CData/*3:0*/ __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter;
    // Body
    __Vdly__Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit 
        = vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit;
    __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter 
        = vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter;
    __Vdly__Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit 
        = ((IData)(vlTOPp->IO_Rst_I) ? ((2U & ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit) 
                                               << 1U)) 
                                        | (IData)(vlTOPp->IO_Rx_I))
            : 0U);
    if (vlTOPp->IO_Rst_I) {
        if (VL_UNLIKELY(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag)) {
            VL_WRITEF("UartRTL: Parsed 1 byte %x\n",
                      8,vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data);
        }
        vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag 
            = vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment;
    } else {
        vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag = 0U;
    }
    if (vlTOPp->IO_Rst_I) {
        if ((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
            if ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))) {
                if (vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment) {
                    __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter 
                        = ((8U > (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter))
                            ? (0xfU & ((IData)(1U) 
                                       + (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter)))
                            : 0U);
                }
            }
        } else {
            if ((1U & (~ (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state)))) {
                __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter = 0U;
            }
        }
    } else {
        __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter = 0U;
    }
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter 
        = ((IData)(vlTOPp->IO_Rst_I) ? (0xffU & ((2U 
                                                  & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                                   ? 
                                                  ((0x56U 
                                                    > (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    ? 
                                                   ((IData)(1U) 
                                                    + (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    : 0U)
                                                   : 
                                                  ((0x56U 
                                                    > (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    ? 
                                                   ((IData)(1U) 
                                                    + (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    : 0U))
                                                  : 
                                                 ((1U 
                                                   & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                                   ? 
                                                  ((0x2bU 
                                                    > (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    ? 
                                                   ((IData)(1U) 
                                                    + (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter))
                                                    : 0U)
                                                   : 0U)))
            : 0U);
    if (vlTOPp->IO_Rst_I) {
        if (vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment) {
            vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data 
                = (((~ ((IData)(1U) << (7U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter)))) 
                    & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data)) 
                   | ((1U & ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit) 
                             >> 1U)) << (7U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter))));
        }
    } else {
        vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data = 0U;
    }
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state 
        = ((IData)(vlTOPp->IO_Rst_I) ? (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_next_state)
            : 0U);
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit 
        = __Vdly__Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit;
    vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter 
        = __Vdly__Top__DOT__uart_rx_instance__DOT__reg_bit_counter;
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

void VTop::_eval(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_eval\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if ((((IData)(vlTOPp->IO_Clk_I) & (~ (IData)(vlTOPp->__Vclklast__TOP__IO_Clk_I))) 
         | ((~ (IData)(vlTOPp->IO_Rst_I)) & (IData)(vlTOPp->__Vclklast__TOP__IO_Rst_I)))) {
        vlTOPp->_sequent__TOP__2(vlSymsp);
        vlTOPp->__Vm_traceActivity[1U] = 1U;
    }
    // Final
    vlTOPp->__Vclklast__TOP__IO_Clk_I = vlTOPp->IO_Clk_I;
    vlTOPp->__Vclklast__TOP__IO_Rst_I = vlTOPp->IO_Rst_I;
}

VL_INLINE_OPT QData VTop::_change_request(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_change_request\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    return (vlTOPp->_change_request_1(vlSymsp));
}

VL_INLINE_OPT QData VTop::_change_request_1(VTop__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_change_request_1\n"); );
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    // Change detection
    QData __req = false;  // Logically a bool
    return __req;
}

#ifdef VL_DEBUG
void VTop::_eval_debug_assertions() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VTop::_eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((IO_Rst_I & 0xfeU))) {
        Verilated::overWidthError("IO_Rst_I");}
    if (VL_UNLIKELY((IO_Clk_I & 0xfeU))) {
        Verilated::overWidthError("IO_Clk_I");}
    if (VL_UNLIKELY((IO_Rx_I & 0xfeU))) {
        Verilated::overWidthError("IO_Rx_I");}
}
#endif  // VL_DEBUG
