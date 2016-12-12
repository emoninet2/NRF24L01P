/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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


#if defined (NRF24L01P_PORT_MBED)

NRF24L01pPortDriver::NRF24L01pPortDriver() {
}

NRF24L01pPortDriver::NRF24L01pPortDriver(const NRF24L01pPortDriver& orig) {
}

NRF24L01pPortDriver::~NRF24L01pPortDriver() {
}

void NRF24L01pPortDriver::Port_Initialize(void){

}
void NRF24L01pPortDriver::Port_DeInitialize(void){

}
void NRF24L01pPortDriver::Port_CE_pin(bool state){

}
void NRF24L01pPortDriver::Port_CSN_pin(bool state){

}
int NRF24L01pPortDriver::Port_SPI_Transcieve(uint8_t *dataOut, uint8_t *dataIn, unsigned int size){

}
int NRF24L01pPortDriver::Port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size){

}
void NRF24L01pPortDriver::Port_Delay_us(unsigned int us){

}
void NRF24L01pPortDriver::Port_Delay_ms(unsigned int ms){

}

void NRF24L01pPortDriver::Port_Error(void){

}

#endif