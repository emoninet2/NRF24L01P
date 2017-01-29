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

#include "NRF24L01pPort.h"


#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdlib.h>
#include <stdio.h>

#define nrf24l01p_csn_pin 11
#define nrf24l01p_ce_pin 26

#define nrf24l01p_SPI 0



NRF24L01pPort::NRF24L01pPort() {
}

NRF24L01pPort::NRF24L01pPort(const NRF24L01pPort& orig) {
}

NRF24L01pPort::~NRF24L01pPort() {
}

void NRF24L01pPort::port_Initialize(){

    setenv("WIRINGPI_GPIOMEM", "1", 1);
    wiringPiSetup () ;
    pinMode(nrf24l01p_csn_pin ,OUTPUT);//CSN pin as OUTPUT
    pinMode(nrf24l01p_ce_pin ,OUTPUT);//CE pin as OUTPUT
    wiringPiSPISetup(nrf24l01p_SPI,500000);

    digitalWrite(nrf24l01p_csn_pin,LOW);
    digitalWrite(nrf24l01p_ce_pin,LOW);
}
void NRF24L01pPort::port_DeInitialize(){
    
}
void NRF24L01pPort::port_Pin_CE(bool val){
    digitalWrite(nrf24l01p_ce_pin,val);
}
void NRF24L01pPort::port_Pin_CSN(bool val){
    digitalWrite(nrf24l01p_csn_pin,val);
}
int NRF24L01pPort::port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size){
    return wiringPiSPIDataRW(nrf24l01p_SPI, (unsigned char*)dataInOut,size);
}
void NRF24L01pPort::port_DelayMs(unsigned int ms){
    delay(ms);
}
void NRF24L01pPort::port_DelayUs(unsigned int us){
    delayMicroseconds(us);
}
unsigned int NRF24L01pPort::port_ClockMs(){
    return millis();
}