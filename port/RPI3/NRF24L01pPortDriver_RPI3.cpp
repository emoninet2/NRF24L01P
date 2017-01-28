/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pPortDriver.cpp
 * Author: emon1
 * 
 * Created on December 11, 2016, 7:59 PM
 */

#include "../NRF24L01pPortDriver.h"
#include "../NRF24L01pPort.h"


#if defined (NRF24L01P_PORT_RPI3)

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdlib.h>
#include <stdio.h>

#define nrf24l01p_csn_pin 11
#define nrf24l01p_ce_pin 26

#define nrf24l01p_SPI 1


NRF24L01pPortDriver::NRF24L01pPortDriver() {
}

NRF24L01pPortDriver::NRF24L01pPortDriver(const NRF24L01pPortDriver& orig) {
}

NRF24L01pPortDriver::~NRF24L01pPortDriver() {
}

void NRF24L01pPortDriver::Port_Initialize(void){

    //setenv("WIRINGPI_GPIOMEM", "1", 1);
    wiringPiSetup () ;
    pinMode(nrf24l01p_csn_pin ,OUTPUT);//CSN pin as OUTPUT
    pinMode(nrf24l01p_ce_pin ,OUTPUT);//CE pin as OUTPUT
    wiringPiSPISetup(nrf24l01p_SPI,500000);

    digitalWrite(nrf24l01p_csn_pin,LOW);
    digitalWrite(nrf24l01p_ce_pin,LOW);
}
void NRF24L01pPortDriver::Port_DeInitialize(void){

}
void NRF24L01pPortDriver::Port_CE_pin(bool state){
    digitalWrite(nrf24l01p_ce_pin,state);
}
void NRF24L01pPortDriver::Port_CSN_pin(bool state){
    digitalWrite(nrf24l01p_csn_pin,state);
}
int NRF24L01pPortDriver::Port_SPI_Transcieve(uint8_t *dataOut, uint8_t *dataIn, unsigned int size){

}
int NRF24L01pPortDriver::Port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size){
    return wiringPiSPIDataRW(nrf24l01p_SPI, (unsigned char*)dataInOut,size);
}
void NRF24L01pPortDriver::Port_Delay_us(unsigned int us){
    delayMicroseconds(us);
}
void NRF24L01pPortDriver::Port_Delay_ms(unsigned int ms){
    delay(ms);
}

void NRF24L01pPortDriver::Port_Error(void){

}

#endif