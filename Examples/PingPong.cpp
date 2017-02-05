/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: emon1
 *
 * Created on February 4, 2017, 7:51 PM
 */

#include <stdio.h>
#include <stdint.h>
#include <cstdlib>
#include <string.h>

#include "NRF24L01p/NRF24L01p.h"
using namespace std;


#define PING_NODE 1
#define PONG_NODE 0


NRF24L01p Radio;

NRF24L01p::RadioConfig_t RadioConfig;
NRF24L01p::RxPipeConfig_t RxPipeConfig[6];
void RadioReset(){

    RadioConfig.DataReadyInterruptEnabled = 0;
    RadioConfig.DataSentInterruptFlagEnabled = 0;
    RadioConfig.MaxRetryInterruptFlagEnabled = 0;
    RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPower = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;

#if  PING_NODE == 1
    
    RxPipeConfig[0].address = 0xAABBCCDDEE;
    RxPipeConfig[1].address = 0x6565656501;
    RxPipeConfig[2].address = 0x6565656502;
    RxPipeConfig[3].address = 0x6565656503;
    RxPipeConfig[4].address = 0x6565656509;
    RxPipeConfig[5].address = 0x6565656505;

#elif  PONG_NODE == 1
    
    RxPipeConfig[0].address = 0x11223344EE;
    RxPipeConfig[1].address = 0x9A4524CE01;
    RxPipeConfig[2].address = 0x9A4524CE02;
    RxPipeConfig[3].address = 0x9A4524CE03;
    RxPipeConfig[4].address = 0x9A4524CE09;
    RxPipeConfig[5].address = 0x9A4524CE05;
    
#endif   
    
    int i;
    for(i=0;i<6;i++){
        RxPipeConfig[i].PipeEnabled = 1;
        RxPipeConfig[i].autoAckEnabled = 1;
        RxPipeConfig[i].dynamicPayloadEnabled = 1;
    }
    

    Radio.ResetConfigValues(&RadioConfig, RxPipeConfig);
}




#if  PING_NODE == 1
    

/*
 * 
 */
int main(int argc, char** argv) {
    printf("TX NODE\r\n");
    RadioReset();

    char myMesg[32];
    NRF24L01p::Payload_t payload;
    
    payload.UseAck = 1;
    payload.address = 0x9A4524CE01;
    payload.data = (uint8_t*)myMesg;
    payload.length = strlen(myMesg);
    payload.retransmitCount = 15;
    
    printf("DYNPD : %x\r\n", Radio.read_register(0x1c));
    printf("FEATURE : %x\r\n", Radio.read_register(0x1d));
    printf("FIFO : %x\r\n", Radio.read_register(0x17));
    printf("RF_SETUP : %x\r\n", Radio.read_register(0x06));
    
    int i = 0;
    while(1){
        sprintf((char*)payload.data, "PING -> %d", i++);
        payload.length = strlen((char*)payload.data);
        Radio.TransmitPayload(&payload);
        
        
        if(Radio.readable()){
            uint8_t RxData[32];
            NRF24L01p::Payload_t payload;
            payload.data = RxData;
            
            Radio.clear_data_ready_flag();
            Radio.readPayload(&payload);
            payload.data[payload.length] = '\0';
            printf("DATA P%d %d: %s\r\n", payload.pipe, payload.length, payload.data);
            Radio.flush_rx();
      
        }
        
        
        
    }
    
    
    
    
    return 0;
}

#elif  PONG_NODE == 1

/*
 * 
 */
int main(int argc, char** argv) {
    printf("RX NODE\r\n");
    RadioReset();
    
    printf("DYNPD : %x\r\n", Radio.read_register(0x1c));
    printf("FEATURE : %x\r\n", Radio.read_register(0x1d));
    printf("FIFO : %x\r\n", Radio.read_register(0x17));
    printf("RF_SETUP : %x\r\n", Radio.read_register(0x06));

    char myMesg[32];
    NRF24L01p::Payload_t payload;
    
    payload.UseAck = 1;
    payload.address = 0x9A4524CE01;
    payload.data = (uint8_t*)myMesg;
    payload.length = strlen(myMesg);
    payload.retransmitCount = 15;
    payload.pipe = NRF24L01p::PIPE_P1;
    
    int i=0;
    sprintf((char*)payload.data, "PONG -> %d", i++);
    payload.length = strlen((char*)payload.data);
    Radio.writeAckPayload(&payload);
    while(1){
        //Radio.port_DelayMs(1000);

        if(Radio.readable()){

            uint8_t RxData[32];
            NRF24L01p::Payload_t payload;
            payload.data = RxData;
            
            Radio.clear_data_ready_flag();
            Radio.readPayload(&payload);
            payload.data[payload.length] = '\0';
            printf("DATA P%d %d: %s\r\n", payload.pipe, payload.length, payload.data);
            Radio.flush_rx();
            
            
            sprintf((char*)payload.data, "PONG -> %d", i++);
            payload.length = strlen((char*)payload.data);
            Radio.writeAckPayload(&payload);
      
        }
        //printf("status : %x\r\n", Radio.get_status());
        //printf("config : %x\r\n", Radio.read_register(0));
        //printf("%c[2K", 27);
    }
    
    
    return 0;
}
    
    
#endif   
    



