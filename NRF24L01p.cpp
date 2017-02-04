/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01p.cpp
 * Author: emon1
 * 
 * Created on January 29, 2017, 7:10 AM
 */

#include "NRF24L01p.h"

NRF24L01p::NRF24L01p() {
    
    port_Initialize();
    ResetConfigValues();
    
    
    port_Pin_CE(0);
    port_Pin_CSN(0);

    port_DelayMs(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clear_data_ready_flag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);


    
}

NRF24L01p::NRF24L01p(const NRF24L01p& orig) {
}

NRF24L01p::~NRF24L01p() {
}


void NRF24L01p::ResetConfigValues(){

    RadioConfig.DataReadyInterruptEnabled = 0;
    RadioConfig.DataSentInterruptFlagEnabled = 0;
    RadioConfig.MaxRetryInterruptFlagEnabled = 0;
    RadioConfig.Crc = CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPower = RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;

    int i;
    for(i=0;i<6;i++){
        RxPipeConfig[i].PipeEnabled = 1;
        RxPipeConfig[i].autoAckEnabled = 1;
        RxPipeConfig[i].dynamicPayloadEnabled = 1;
    }
    
    RxPipeConfig[0].address = 0xAABBCCDDEE;
    RxPipeConfig[1].address = 0x6565656501;
    RxPipeConfig[2].address = 0x6565656502;
    RxPipeConfig[3].address = 0x6565656503;
    RxPipeConfig[4].address = 0x6565656504;
    RxPipeConfig[5].address = 0x6565656505;
    
    
    enable_dynamic_payload(RadioConfig.FeatureDynamicPayloadEnabled);
    enable_payload_with_ack(RadioConfig.FeaturePayloadWithAckEnabled);
    enable_dynamic_payload_with_no_ack(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled);
    set_auto_retransmission_count(RadioConfig.AutoReTransmissionCount);
    set_auto_retransmission_delay(RadioConfig.AutoReTransmitDelayX250us);        
    set_DataRate(RadioConfig.datarate);
    
    
    for(i=0;i<6;i++){
        enable_rx_on_pipe((pipe_t)i,RxPipeConfig[i].PipeEnabled );
        enable_auto_ack((pipe_t)i,RxPipeConfig[i].autoAckEnabled );
        enable_dynamic_payload_pipe((pipe_t)i,RxPipeConfig[i].dynamicPayloadEnabled);
        set_RX_pipe_address((pipe_t)i,RxPipeConfig[i].address);
    }
}

void NRF24L01p::RadioMode(NRF24L01p::RadioState_t mode){
    switch(mode){
        case MODE_POWER_DOWN: {
            power_down();
            port_Pin_CE(0);
            RadioState = MODE_POWER_DOWN;
            break;
        }
        case MODE_STANDBY: {
            if(RadioState == MODE_POWER_DOWN){
                    power_up();
                    port_DelayUs(_NRF24L01P_TIMING_Tpd2stby_us);
            }
            else{
                    port_Pin_CE(0);
            }
            RadioState = MODE_STANDBY;
            break;
        }
        case MODE_RX: {
            if(RadioState != MODE_RX){
                port_Pin_CE(0);
                rx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_RX;
            }
            break;
        }
        case MODE_TX: {
            if(RadioState != MODE_TX){
                port_Pin_CE(0);
                tx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_TX;
            }
            break;
        }		
    }
}


bool NRF24L01p::readable(){
    return get_data_ready_flag() || !get_fifo_flag_rx_empty(); 
}
bool NRF24L01p::writable(){
    !get_fifo_flag_tx_empty();
}
bool NRF24L01p::readableOnPipe(pipe_t pipe){
    
}


NRF24L01p::ErrorStatus_t NRF24L01p::writePayload(Payload_t *payload){
    set_TX_pipe_address(payload->address);
    if(payload->UseAck == 1){
        write_tx_payload(payload->data,payload->length);
    }else{
        if(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled == 1){
            write_tx_payload_noack(payload->data,payload->length); 
        }
    }
}

NRF24L01p::ErrorStatus_t NRF24L01p::writeAckPayload(Payload_t *payload){
    write_ack_payload(payload->pipe, payload->data, payload->length);
}
NRF24L01p::ErrorStatus_t NRF24L01p::readPayload(Payload_t *payload){
    
    payload->pipe = get_rx_payload_pipe();
    
    if(payload->pipe>=0 && payload->pipe<=5){
        if(RadioConfig.FeatureDynamicPayloadEnabled == 1){
            payload->length = read_rx_payload_width();
        }else{
            payload->length = get_RX_pipe_width(payload->pipe);
        }
    read_rx_payload(payload->data,payload->length);
    }

}


NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayload(Payload_t *payload){
    if(TxPipeAddress != payload->address){
        set_TX_pipe_address(payload->address);
        TxPipeAddress = payload->address;
    }

    if(payload->UseAck){
        
        if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
            enable_auto_ack(PIPE_P0, 1);
        }
        if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
            enable_rx_on_pipe(PIPE_P0, 1);
        }
        set_RX_pipe_address(PIPE_P0, payload->address);
        
        
        writeAckPayload(payload);
        
        RadioState_t originalState = RadioState;
        ErrorStatus_t retval;
        if(writable()){
            clear_data_sent_flag();
            while(1){
                RadioMode(MODE_TX);   
                RadioMode(MODE_STANDBY);
                
                if(get_data_sent_flag()){
                    retval = SUCCESS;
                    break;
                }
                if(get_max_retry_flag()){
                    clear_max_retry_flag();
                    if(get_plos_count()>=payload->retransmitCount){
                        set_frequency_offset(RadioConfig.frequencyOffset);
                        retval = RETRANSMIT_REACHED_MAX_PLOS;
                        break;
                    }
                }
            }
        }

        RadioMode(originalState);
        
        set_RX_pipe_address(PIPE_P0, RxPipeConfig[PIPE_P0].address);
        if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
            enable_auto_ack(PIPE_P0, 0);
        }
        if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
            enable_rx_on_pipe(PIPE_P0, 0);
        }
    }else{
        set_TX_pipe_address(payload->address);
        writeAckPayload(payload);
        RadioState_t originalState = RadioState;
        ErrorStatus_t retval;
        if(writable()){
            clear_data_sent_flag();
            while(1){
                RadioMode(MODE_TX);   
                RadioMode(MODE_STANDBY);
                if(get_data_sent_flag()){
                    retval = SUCCESS;
                break;
                }
            }
        }
        RadioMode(originalState);
    }
}
NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayload(Payload_t *payload){
    clear_data_ready_flag();
    
    readPayload(payload);
}