/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pPort.cpp
 * Author: emon1
 * 
 * Created on January 29, 2017, 8:03 AM
 */

#include "../NRF24L01pPortConfig.h"
#include "../NRF24L01pPort.h"

#if (NRF24L01pPort_mbed == 1)

#define NRF24L01p_MBED_CE_PIN           D9
#define NRF24L01p_MBED_CSN_PIN          D10
#define NRF24L01p_MBED_SPI_MOSI_PIN     SPI_MOSI//D12
#define NRF24L01p_MBED_SPI_MISO_PIN     SPI_MISO//D11
#define NRF24L01p_MBED_SPI_SCK_PIN      SPI_SCK//D13
#define NRF24L01p_MBED_IRQ_PIN          D7


#include "mbed.h"

#include <stdlib.h>
#include <stdio.h>

DigitalOut nrf24l01p_ce(NRF24L01p_MBED_CE_PIN);
DigitalOut nrf24l01p_csn(NRF24L01p_MBED_CSN_PIN);
SPI nrf24l01p_spi(NRF24L01p_MBED_SPI_MOSI_PIN, NRF24L01p_MBED_SPI_MISO_PIN, NRF24L01p_MBED_SPI_SCK_PIN);
Timer t;

NRF24L01pPort::NRF24L01pPort() {
}

NRF24L01pPort::NRF24L01pPort(const NRF24L01pPort& orig) {
}

NRF24L01pPort::~NRF24L01pPort() {
}

void NRF24L01pPort::port_Initialize(){
    t.start ();
}
void NRF24L01pPort::port_DeInitialize(){
    
}
void NRF24L01pPort::port_Pin_CE(bool val){
    nrf24l01p_ce = val;
}
void NRF24L01pPort::port_Pin_CSN(bool val){
    nrf24l01p_csn = val;
}
int NRF24L01pPort::port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size){
    int i=0;
    for(i=0;i<size;i++){
        dataInOut[i] = nrf24l01p_spi.write(dataInOut[i]);   
    }
}
void NRF24L01pPort::port_DelayMs(int ms){
    wait_ms(ms);
}
void NRF24L01pPort::port_DelayUs(int us){
    wait_us(us);
}
unsigned int NRF24L01pPort::port_ClockMs(){
    return t.read_ms ();
}

#endif