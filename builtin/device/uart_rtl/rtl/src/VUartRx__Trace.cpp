// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VUartRx__Syms.h"


void VUartRx::traceChgTop0(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    {
        vlTOPp->traceChgSub0(userp, tracep);
    }
}

void VUartRx::traceChgSub0(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode + 1);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        if (VL_UNLIKELY(vlTOPp->__Vm_traceActivity[1U])) {
            tracep->chgCData(oldp+0,(vlTOPp->UartRx__DOT__reg_fsm_state),2);
            tracep->chgCData(oldp+1,(((2U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                       ? ((1U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                           ? ((8U == (IData)(vlTOPp->UartRx__DOT__reg_bit_counter))
                                               ? 2U
                                               : 3U)
                                           : ((IData)(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment)
                                               ? 0U
                                               : 2U))
                                       : ((1U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                           ? ((IData)(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment)
                                               ? ((2U 
                                                   & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                                   ? 0U
                                                   : 3U)
                                               : 1U)
                                           : ((2U & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                               ? 0U
                                               : 1U)))),2);
            tracep->chgBit(oldp+2,(vlTOPp->UartRx__DOT__reg_data_bit_mid_moment));
            tracep->chgBit(oldp+3,(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment));
            tracep->chgBit(oldp+4,(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment));
            tracep->chgCData(oldp+5,(vlTOPp->UartRx__DOT__reg_clk_counter),8);
            tracep->chgCData(oldp+6,(vlTOPp->UartRx__DOT__reg_bit_counter),4);
            tracep->chgCData(oldp+7,(vlTOPp->UartRx__DOT__reg_recv_data_bit),2);
            tracep->chgBit(oldp+8,(vlTOPp->UartRx__DOT__reg_rx_done_flag));
            tracep->chgCData(oldp+9,(vlTOPp->UartRx__DOT__reg_recv_data),8);
        }
        tracep->chgBit(oldp+10,(vlTOPp->IO_Rst_I));
        tracep->chgBit(oldp+11,(vlTOPp->IO_Clk_I));
        tracep->chgBit(oldp+12,(vlTOPp->IO_Rx_I));
        tracep->chgBit(oldp+13,(vlTOPp->IO_RxDone_O));
        tracep->chgCData(oldp+14,(vlTOPp->IO_RxData_O),8);
        tracep->chgBit(oldp+15,(vlTOPp->IO_Debug_Sample_O));
    }
}

void VUartRx::traceCleanup(void* userp, VerilatedVcd* /*unused*/) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlSymsp->__Vm_activity = false;
        vlTOPp->__Vm_traceActivity[0U] = 0U;
        vlTOPp->__Vm_traceActivity[1U] = 0U;
    }
}
