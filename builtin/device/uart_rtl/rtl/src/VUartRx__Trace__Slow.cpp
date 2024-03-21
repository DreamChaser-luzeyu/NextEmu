// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VUartRx__Syms.h"


//======================

void VUartRx::trace(VerilatedVcdC* tfp, int, int) {
    tfp->spTrace()->addInitCb(&traceInit, __VlSymsp);
    traceRegister(tfp->spTrace());
}

void VUartRx::traceInit(void* userp, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    if (!Verilated::calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
                        "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->module(vlSymsp->name());
    tracep->scopeEscape(' ');
    VUartRx::traceInitTop(vlSymsp, tracep);
    tracep->scopeEscape('.');
}

//======================


void VUartRx::traceInitTop(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceInitSub0(userp, tracep);
    }
}

void VUartRx::traceInitSub0(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    const int c = vlSymsp->__Vm_baseCode;
    if (false && tracep && c) {}  // Prevent unused
    // Body
    {
        tracep->declBit(c+11,"IO_Rst_I", false,-1);
        tracep->declBit(c+12,"IO_Clk_I", false,-1);
        tracep->declBit(c+13,"IO_Rx_I", false,-1);
        tracep->declBit(c+14,"IO_RxDone_O", false,-1);
        tracep->declBus(c+15,"IO_RxData_O", false,-1, 7,0);
        tracep->declBit(c+16,"IO_Debug_Sample_O", false,-1);
        tracep->declBus(c+17,"UartRx Param_BaurdRate", false,-1, 31,0);
        tracep->declBus(c+18,"UartRx Param_ClkFreq", false,-1, 31,0);
        tracep->declBus(c+19,"UartRx Param_PayloadBits", false,-1, 31,0);
        tracep->declBit(c+11,"UartRx IO_Rst_I", false,-1);
        tracep->declBit(c+12,"UartRx IO_Clk_I", false,-1);
        tracep->declBit(c+13,"UartRx IO_Rx_I", false,-1);
        tracep->declBit(c+14,"UartRx IO_RxDone_O", false,-1);
        tracep->declBus(c+15,"UartRx IO_RxData_O", false,-1, 7,0);
        tracep->declBit(c+16,"UartRx IO_Debug_Sample_O", false,-1);
        tracep->declBus(c+19,"UartRx Param_ClkCounterBitWidth", false,-1, 31,0);
        tracep->declBus(c+20,"UartRx Param_BitCounterBitWidth", false,-1, 31,0);
        tracep->declBus(c+21,"UartRx clk_per_bit", false,-1, 31,0);
        tracep->declBus(c+22,"UartRx Param_ClkPerBit", false,-1, 7,0);
        tracep->declBus(c+23,"UartRx Param_StateIDLE", false,-1, 1,0);
        tracep->declBus(c+24,"UartRx Param_StateStartBit", false,-1, 1,0);
        tracep->declBus(c+25,"UartRx Param_StateDataBits", false,-1, 1,0);
        tracep->declBus(c+26,"UartRx Param_StateStopBit", false,-1, 1,0);
        tracep->declBus(c+1,"UartRx reg_fsm_state", false,-1, 1,0);
        tracep->declBus(c+2,"UartRx reg_fsm_next_state", false,-1, 1,0);
        tracep->declBit(c+3,"UartRx reg_data_bit_mid_moment", false,-1);
        tracep->declBit(c+4,"UartRx reg_start_bit_mid_moment", false,-1);
        tracep->declBit(c+5,"UartRx reg_stop_bit_mid_moment", false,-1);
        tracep->declBus(c+6,"UartRx reg_clk_counter", false,-1, 7,0);
        tracep->declBus(c+7,"UartRx reg_bit_counter", false,-1, 3,0);
        tracep->declBus(c+8,"UartRx reg_recv_data_bit", false,-1, 1,0);
        tracep->declBit(c+9,"UartRx reg_rx_done_flag", false,-1);
        tracep->declBus(c+10,"UartRx reg_recv_data", false,-1, 7,0);
    }
}

void VUartRx::traceRegister(VerilatedVcd* tracep) {
    // Body
    {
        tracep->addFullCb(&traceFullTop0, __VlSymsp);
        tracep->addChgCb(&traceChgTop0, __VlSymsp);
        tracep->addCleanupCb(&traceCleanup, __VlSymsp);
    }
}

void VUartRx::traceFullTop0(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceFullSub0(userp, tracep);
    }
}

void VUartRx::traceFullSub0(void* userp, VerilatedVcd* tracep) {
    VUartRx__Syms* __restrict vlSymsp = static_cast<VUartRx__Syms*>(userp);
    VUartRx* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        tracep->fullCData(oldp+1,(vlTOPp->UartRx__DOT__reg_fsm_state),2);
        tracep->fullCData(oldp+2,(((2U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                    ? ((1U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                        ? ((8U == (IData)(vlTOPp->UartRx__DOT__reg_bit_counter))
                                            ? 2U : 3U)
                                        : ((IData)(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment)
                                            ? 0U : 2U))
                                    : ((1U & (IData)(vlTOPp->UartRx__DOT__reg_fsm_state))
                                        ? ((IData)(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment)
                                            ? ((2U 
                                                & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                                ? 0U
                                                : 3U)
                                            : 1U) : 
                                       ((2U & (IData)(vlTOPp->UartRx__DOT__reg_recv_data_bit))
                                         ? 0U : 1U)))),2);
        tracep->fullBit(oldp+3,(vlTOPp->UartRx__DOT__reg_data_bit_mid_moment));
        tracep->fullBit(oldp+4,(vlTOPp->UartRx__DOT__reg_start_bit_mid_moment));
        tracep->fullBit(oldp+5,(vlTOPp->UartRx__DOT__reg_stop_bit_mid_moment));
        tracep->fullCData(oldp+6,(vlTOPp->UartRx__DOT__reg_clk_counter),8);
        tracep->fullCData(oldp+7,(vlTOPp->UartRx__DOT__reg_bit_counter),4);
        tracep->fullCData(oldp+8,(vlTOPp->UartRx__DOT__reg_recv_data_bit),2);
        tracep->fullBit(oldp+9,(vlTOPp->UartRx__DOT__reg_rx_done_flag));
        tracep->fullCData(oldp+10,(vlTOPp->UartRx__DOT__reg_recv_data),8);
        tracep->fullBit(oldp+11,(vlTOPp->IO_Rst_I));
        tracep->fullBit(oldp+12,(vlTOPp->IO_Clk_I));
        tracep->fullBit(oldp+13,(vlTOPp->IO_Rx_I));
        tracep->fullBit(oldp+14,(vlTOPp->IO_RxDone_O));
        tracep->fullCData(oldp+15,(vlTOPp->IO_RxData_O),8);
        tracep->fullBit(oldp+16,(vlTOPp->IO_Debug_Sample_O));
        tracep->fullIData(oldp+17,(0x2580U),32);
        tracep->fullIData(oldp+18,(0xf4240U),32);
        tracep->fullIData(oldp+19,(8U),32);
        tracep->fullIData(oldp+20,(4U),32);
        tracep->fullIData(oldp+21,(0x68U),32);
        tracep->fullCData(oldp+22,(0x68U),8);
        tracep->fullCData(oldp+23,(0U),2);
        tracep->fullCData(oldp+24,(1U),2);
        tracep->fullCData(oldp+25,(3U),2);
        tracep->fullCData(oldp+26,(2U),2);
    }
}
