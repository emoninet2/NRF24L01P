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

NRF24L01p radio;

/*
 * 
 */
int main(int argc, char** argv) {
    radio.initialize();
    
    printf("the status is %x\r\n", radio.get_status());
    printf("the data rate is %x\r\n", radio.get_DataRate());
    
    radio.set_RX_pipe_address(NRF24L01p::PIPE_P1, 0x6f6d6f6e31);

    radio.flush_rx();
    radio.flush_tx();
    
    radio.RadioMode(NRF24L01p::MODE_STANDBY);

    radio.PTX();
    radio.PTX();
    radio.PTX();
    radio.PTX();
    radio.PTX();
    radio.PTX();
    radio.PTX();

    
    radio.RadioMode(NRF24L01p::MODE_RX);
    while(1){
        radio.Port_Delay_ms(100);
        NRF24L01p::Payload_t pld;
        radio.PRX();
        
        if(radio.fifo_read(&radio.RxFifo, &pld) == 0){
            printf("%d : %s\r\n", pld.RxPipe, pld.data);
        }
        
        //printf("%x\r\n", radio.get_status());
        
        
    }
    
    
    
    printf("done man\r\n");
    
    return 0;
    
    
}

