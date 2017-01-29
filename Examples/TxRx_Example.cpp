/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: emon1
 *
 * Created on December 11, 2016, 7:40 PM
 */

#include <cstdlib>
#include "NRF24L01p/NRF24L01p.h"
#include <stdio.h>
#include <string.h>
using namespace std;


//#define TX
#define RX


NRF24L01p radio;

/*
 * 
 */
int main(int argc, char** argv) {
    radio.initialize();

    
#if defined(PING)    
    radio.set_RX_pipe_address(NRF24L01p::PIPE_P1, 0x6f6d6f6e31);
#endif
#if defined(PONG)    
    radio.set_RX_pipe_address(NRF24L01p::PIPE_P1, 0x6f6d6f6e32);
    printf("PONG MODE\r\n");
#endif    
    
    
    radio.flush_rx();
    radio.flush_tx();
    radio.RadioMode(NRF24L01p::MODE_STANDBY);
    radio.RadioMode(NRF24L01p::MODE_RX);
    
    while(1){
        
#if defined(TX)          
        
        radio.port_DelayMs(1000);
        printf("sending PING\r\n");
        NRF24L01p::Payload_t myPayload;
        NRF24L01p::Payload_t pld;
        memcpy(myPayload.data, "PING", sizeof("PING"));
        myPayload.len = 4;
        myPayload.TxAddr = 0x6f6d6f6e32;
        int ret = radio.TransmitPacket(&myPayload);
        if(ret ==0) printf("\tgot ack\r\n");
        
#endif
        
#if defined(RX)    
        radio.port_DelayMs(100);
        NRF24L01p::Payload_t pld;
        

        int ret = radio.ReceivePacket(&pld);
        if(ret == 0){
            printf("got packet\r\n");
        }

#endif   
     
    }
    

    printf("done man\r\n");
    
    return 0;
    
    
}

