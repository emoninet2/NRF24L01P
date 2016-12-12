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


//#define PING
#define PONG




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
#endif    
    
    
    radio.flush_rx();
    radio.flush_tx();
    radio.RadioMode(NRF24L01p::MODE_STANDBY);
    radio.RadioMode(NRF24L01p::MODE_RX);
    
    while(1){
        
#if defined(PING)          
        
        radio.Port_Delay_ms(1000);
        printf("sending PING\r\n");
        NRF24L01p::Payload_t myPayload;
        NRF24L01p::Payload_t pld;
        memcpy(myPayload.data, "PING", sizeof("PING"));
        myPayload.TxAddr = 0x6f6d6f6e32;
        int ret = radio.fifo_write(&radio.TxFifo, &myPayload);
        
        radio.PTX();
        radio.PRX();
        
        if(radio.fifo_read(&radio.RxFifo, &pld) == 0){
            printf("%s\r\n", pld.data);
        }
        radio.flush_rx();
        radio.flush_tx();
#endif
        
#if defined(PONG)    
        radio.Port_Delay_ms(10);
        NRF24L01p::Payload_t pld;
        

        if(radio.fifo_read(&radio.RxFifo, &pld) == 0){
            printf("%s\r\n", pld.data);
            
            NRF24L01p::Payload_t myPayload;
            memcpy(myPayload.data, "PONG", sizeof("PONG"));
            myPayload.TxAddr = 0x6f6d6f6e31;
            int ret = radio.fifo_write(&radio.TxFifo, &myPayload);
            
        }
        
        radio.PRX();
        radio.PTX();
        
        radio.flush_rx();
        radio.flush_tx();
#endif   
    
        radio.flush_rx();
        radio.flush_tx();    
    }
    
    
    
    printf("done man\r\n");
    
    return 0;
    
    
}

