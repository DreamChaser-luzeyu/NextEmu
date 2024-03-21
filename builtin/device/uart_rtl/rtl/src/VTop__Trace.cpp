// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VTop__Syms.h"


void VTop::traceChgTop0(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    {
        vlTOPp->traceChgSub0(userp, tracep);
    }
}

void VTop::traceChgSub0(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode + 1);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        if (VL_UNLIKELY(vlTOPp->__Vm_traceActivity[1U])) {
            tracep->chgBit(oldp+0,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag));
            tracep->chgCData(oldp+1,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data),8);
            tracep->chgBit(oldp+2,((((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment) 
                                     | (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment)) 
                                    | (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment))));
            tracep->chgCData(oldp+3,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state),2);
            tracep->chgCData(oldp+4,(((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                       ? ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                           ? ((8U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter))
                                               ? 2U
                                               : 3U)
                                           : ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment)
                                               ? 0U
                                               : 2U))
                                       : ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                           ? ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment)
                                               ? ((2U 
                                                   & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                                   ? 0U
                                                   : 3U)
                                               : 1U)
                                           : ((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                               ? 0U
                                               : 1U)))),2);
            tracep->chgBit(oldp+5,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment));
            tracep->chgBit(oldp+6,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment));
            tracep->chgBit(oldp+7,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment));
            tracep->chgCData(oldp+8,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter),8);
            tracep->chgCData(oldp+9,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter),4);
            tracep->chgCData(oldp+10,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit),2);
        }
        tracep->chgIData(oldp+11,(vlTOPp->Ext_RxFIFO),32);
        tracep->chgIData(oldp+12,(vlTOPp->Ext_TxFIFO),32);
        tracep->chgIData(oldp+13,(vlTOPp->Ext_Status),32);
        tracep->chgIData(oldp+14,(vlTOPp->Ext_Ctrl),32);
        tracep->chgBit(oldp+15,(vlTOPp->IO_Rst_I));
        tracep->chgBit(oldp+16,(vlTOPp->IO_Clk_I));
        tracep->chgBit(oldp+17,(vlTOPp->IO_Rx_I));
        tracep->chgBit(oldp+18,(vlTOPp->IO_Tx_O));
    }
}

void VTop::traceCleanup(void* userp, VerilatedVcd* /*unused*/) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlSymsp->__Vm_activity = false;
        vlTOPp->__Vm_traceActivity[0U] = 0U;
        vlTOPp->__Vm_traceActivity[1U] = 0U;
    }
}
