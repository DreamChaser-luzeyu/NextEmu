// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VTop__Syms.h"


//======================

void VTop::trace(VerilatedVcdC* tfp, int, int) {
    tfp->spTrace()->addInitCb(&traceInit, __VlSymsp);
    traceRegister(tfp->spTrace());
}

void VTop::traceInit(void* userp, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    if (!Verilated::calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
                        "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->module(vlSymsp->name());
    tracep->scopeEscape(' ');
    VTop::traceInitTop(vlSymsp, tracep);
    tracep->scopeEscape('.');
}

//======================


void VTop::traceInitTop(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceInitSub0(userp, tracep);
    }
}

void VTop::traceInitSub0(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    const int c = vlSymsp->__Vm_baseCode;
    if (false && tracep && c) {}  // Prevent unused
    // Body
    {
        tracep->declBus(c+12,"Ext_RxFIFO", false,-1, 31,0);
        tracep->declBus(c+13,"Ext_TxFIFO", false,-1, 31,0);
        tracep->declBus(c+14,"Ext_Status", false,-1, 31,0);
        tracep->declBus(c+15,"Ext_Ctrl", false,-1, 31,0);
        tracep->declBit(c+16,"IO_Rst_I", false,-1);
        tracep->declBit(c+17,"IO_Clk_I", false,-1);
        tracep->declBit(c+18,"IO_Rx_I", false,-1);
        tracep->declBit(c+19,"IO_Tx_O", false,-1);
        tracep->declBus(c+20,"Top Param_BaurdRate", false,-1, 31,0);
        tracep->declBus(c+21,"Top Param_ClkFreq", false,-1, 31,0);
        tracep->declBus(c+22,"Top Param_PayloadBits", false,-1, 31,0);
        tracep->declBus(c+12,"Top Ext_RxFIFO", false,-1, 31,0);
        tracep->declBus(c+13,"Top Ext_TxFIFO", false,-1, 31,0);
        tracep->declBus(c+14,"Top Ext_Status", false,-1, 31,0);
        tracep->declBus(c+15,"Top Ext_Ctrl", false,-1, 31,0);
        tracep->declBit(c+16,"Top IO_Rst_I", false,-1);
        tracep->declBit(c+17,"Top IO_Clk_I", false,-1);
        tracep->declBit(c+18,"Top IO_Rx_I", false,-1);
        tracep->declBit(c+19,"Top IO_Tx_O", false,-1);
        tracep->declBit(c+1,"Top wire_rx_done_flag", false,-1);
        tracep->declBus(c+2,"Top wire_rx_data", false,-1, 7,0);
        tracep->declBus(c+20,"Top uart_rx_instance Param_BaurdRate", false,-1, 31,0);
        tracep->declBus(c+21,"Top uart_rx_instance Param_ClkFreq", false,-1, 31,0);
        tracep->declBus(c+22,"Top uart_rx_instance Param_PayloadBits", false,-1, 31,0);
        tracep->declBit(c+16,"Top uart_rx_instance IO_Rst_I", false,-1);
        tracep->declBit(c+17,"Top uart_rx_instance IO_Clk_I", false,-1);
        tracep->declBit(c+18,"Top uart_rx_instance IO_Rx_I", false,-1);
        tracep->declBit(c+1,"Top uart_rx_instance IO_RxDone_O", false,-1);
        tracep->declBus(c+2,"Top uart_rx_instance IO_RxData_O", false,-1, 7,0);
        tracep->declBit(c+3,"Top uart_rx_instance IO_Debug_Sample_O", false,-1);
        tracep->declBus(c+22,"Top uart_rx_instance Param_ClkCounterBitWidth", false,-1, 31,0);
        tracep->declBus(c+23,"Top uart_rx_instance Param_BitCounterBitWidth", false,-1, 31,0);
        tracep->declBus(c+24,"Top uart_rx_instance clk_per_bit", false,-1, 31,0);
        tracep->declBus(c+25,"Top uart_rx_instance Param_ClkPerBit", false,-1, 7,0);
        tracep->declBus(c+26,"Top uart_rx_instance Param_StateIDLE", false,-1, 1,0);
        tracep->declBus(c+27,"Top uart_rx_instance Param_StateStartBit", false,-1, 1,0);
        tracep->declBus(c+28,"Top uart_rx_instance Param_StateDataBits", false,-1, 1,0);
        tracep->declBus(c+29,"Top uart_rx_instance Param_StateStopBit", false,-1, 1,0);
        tracep->declBus(c+4,"Top uart_rx_instance reg_fsm_state", false,-1, 1,0);
        tracep->declBus(c+5,"Top uart_rx_instance reg_fsm_next_state", false,-1, 1,0);
        tracep->declBit(c+6,"Top uart_rx_instance reg_data_bit_mid_moment", false,-1);
        tracep->declBit(c+7,"Top uart_rx_instance reg_start_bit_mid_moment", false,-1);
        tracep->declBit(c+8,"Top uart_rx_instance reg_stop_bit_mid_moment", false,-1);
        tracep->declBus(c+9,"Top uart_rx_instance reg_clk_counter", false,-1, 7,0);
        tracep->declBus(c+10,"Top uart_rx_instance reg_bit_counter", false,-1, 3,0);
        tracep->declBus(c+11,"Top uart_rx_instance reg_recv_data_bit", false,-1, 1,0);
        tracep->declBit(c+1,"Top uart_rx_instance reg_rx_done_flag", false,-1);
        tracep->declBus(c+2,"Top uart_rx_instance reg_recv_data", false,-1, 7,0);
    }
}

void VTop::traceRegister(VerilatedVcd* tracep) {
    // Body
    {
        tracep->addFullCb(&traceFullTop0, __VlSymsp);
        tracep->addChgCb(&traceChgTop0, __VlSymsp);
        tracep->addCleanupCb(&traceCleanup, __VlSymsp);
    }
}

void VTop::traceFullTop0(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceFullSub0(userp, tracep);
    }
}

void VTop::traceFullSub0(void* userp, VerilatedVcd* tracep) {
    VTop__Syms* __restrict vlSymsp = static_cast<VTop__Syms*>(userp);
    VTop* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        tracep->fullBit(oldp+1,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_rx_done_flag));
        tracep->fullCData(oldp+2,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data),8);
        tracep->fullBit(oldp+3,((((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment) 
                                  | (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment)) 
                                 | (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment))));
        tracep->fullCData(oldp+4,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state),2);
        tracep->fullCData(oldp+5,(((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                    ? ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                        ? ((8U == (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter))
                                            ? 2U : 3U)
                                        : ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment)
                                            ? 0U : 2U))
                                    : ((1U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_fsm_state))
                                        ? ((IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment)
                                            ? ((2U 
                                                & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                                ? 0U
                                                : 3U)
                                            : 1U) : 
                                       ((2U & (IData)(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit))
                                         ? 0U : 1U)))),2);
        tracep->fullBit(oldp+6,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_data_bit_mid_moment));
        tracep->fullBit(oldp+7,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_start_bit_mid_moment));
        tracep->fullBit(oldp+8,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_stop_bit_mid_moment));
        tracep->fullCData(oldp+9,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_clk_counter),8);
        tracep->fullCData(oldp+10,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_bit_counter),4);
        tracep->fullCData(oldp+11,(vlTOPp->Top__DOT__uart_rx_instance__DOT__reg_recv_data_bit),2);
        tracep->fullIData(oldp+12,(vlTOPp->Ext_RxFIFO),32);
        tracep->fullIData(oldp+13,(vlTOPp->Ext_TxFIFO),32);
        tracep->fullIData(oldp+14,(vlTOPp->Ext_Status),32);
        tracep->fullIData(oldp+15,(vlTOPp->Ext_Ctrl),32);
        tracep->fullBit(oldp+16,(vlTOPp->IO_Rst_I));
        tracep->fullBit(oldp+17,(vlTOPp->IO_Clk_I));
        tracep->fullBit(oldp+18,(vlTOPp->IO_Rx_I));
        tracep->fullBit(oldp+19,(vlTOPp->IO_Tx_O));
        tracep->fullIData(oldp+20,(0x1c200U),32);
        tracep->fullIData(oldp+21,(0x989680U),32);
        tracep->fullIData(oldp+22,(8U),32);
        tracep->fullIData(oldp+23,(4U),32);
        tracep->fullIData(oldp+24,(0x56U),32);
        tracep->fullCData(oldp+25,(0x56U),8);
        tracep->fullCData(oldp+26,(0U),2);
        tracep->fullCData(oldp+27,(1U),2);
        tracep->fullCData(oldp+28,(3U),2);
        tracep->fullCData(oldp+29,(2U),2);
    }
}
