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
    
    NRF24L01p::Payload_t myPayload;
    NRF24L01p::Payload_t myPayload2;
    
    
    int ret;
    printf("head: %d , tail: %d\r\n", radio.TxFifo.head, radio.TxFifo.tail );
    
    memcpy(myPayload.data, "hello mister", sizeof("hello mister"));
    myPayload.TxAddr = 0x6f6d6f6e31;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
    
    
    memcpy(myPayload.data, "marhaba", sizeof("marhaba"));
    myPayload.TxAddr = 0x6f6d6f6e31;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
    
    memcpy(myPayload.data, "doggy dogy", sizeof("doggy dogy"));
    myPayload.TxAddr = 0x6f6d6f6e31;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
    
    memcpy(myPayload.data, "wof wof wof", sizeof("wof wof wof"));
    myPayload.TxAddr = 0x6f6d6f6e31;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
       
       
    memcpy(myPayload.data, "tada rampam", sizeof("tada rampam"));
    myPayload.TxAddr = 0xABABABABAB;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
    
    memcpy(myPayload.data, "kuttey kameney", sizeof("kuttey kameney"));
    myPayload.TxAddr = 0x6f6d6f6e31;
    ret = radio.fifo_write(&radio.TxFifo, &myPayload);
    printf("ret: %d , head: %d , tail: %d\r\n", ret, radio.TxFifo.head, radio.TxFifo.tail );
    
    

    
    
    radio.flush_rx();
     
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

        
        
    }
    
    
    
    printf("done man\r\n");
    
    return 0;
    
    
}

