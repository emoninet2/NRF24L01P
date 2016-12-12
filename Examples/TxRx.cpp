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
#include "NRF24L01p/NRF24L01pNetwork.h"
#include <stdio.h>
#include <string.h>

using namespace std;

#define TX
//#define RX



NRF24L01pNetwork radio;

/*
 * 
 */
int main(int argc, char** argv) {
    radio.initialize();

     
    radio.set_RX_pipe_address(NRF24L01p::PIPE_P1, 0x6f6d6f6e31);

    
    radio.flush_rx();
    radio.flush_tx();
    radio.RadioMode(NRF24L01p::MODE_STANDBY);
    radio.RadioMode(NRF24L01p::MODE_RX);
    
    int i=0;
    while(1){
        
#if defined(TX)        
        //radio.Port_Delay_ms(10);
        NRF24L01p::Payload_t myPayload;
        
        //memcpy(myPayload.data, "PING", sizeof("PING"));
        sprintf((char*)myPayload.data, "ping %d", i++);
        myPayload.TxAddr = 0x6f6d6f6e32;
        int ret = radio.fifo_write(&radio.TxFifo, &myPayload);
        printf("the write return : %d\r\n", ret);

        NRF24L01p::Payload_t RxPld;
        
        printf("H:%d  T:%d \r\n", radio.RxFifo.head, radio.RxFifo.tail);
        int retval = radio.fifo_read(&radio.RxFifo, &RxPld);
        if(retval == 0){
            printf("%d  H:%d  T:%d    %s\r\n",retval, radio.RxFifo.head, radio.RxFifo.tail,  RxPld.data);
        }
#endif
        
#if defined(RX)
        
        radio.Port_Delay_ms(10);
        NRF24L01p::Payload_t RxPld;
        int retval = radio.fifo_read(&radio.RxFifo, &RxPld);
        if(retval >= 0){
            printf("%s\r\n", RxPld.data);
        }
        
#endif        
        
        
        radio.PRX();
        radio.PTX();
        
        
        
   
    }

    
    return 0;
    
    
}

