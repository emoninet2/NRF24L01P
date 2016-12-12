/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pPortDriver.h
 * Author: emon1
 *
 * Created on December 11, 2016, 7:59 PM
 */

#ifndef NRF24L01PPORTDRIVER_H
#define NRF24L01PPORTDRIVER_H

#include <inttypes.h>
#include <stdio.h>

class NRF24L01pPortDriver {
public:
    NRF24L01pPortDriver();
    NRF24L01pPortDriver(const NRF24L01pPortDriver& orig);
    virtual ~NRF24L01pPortDriver();
    
    void Port_Initialize(void);
    void Port_DeInitialize(void);
    void Port_CE_pin(bool state);
    void Port_CSN_pin(bool state);
    int Port_SPI_Transcieve(uint8_t *dataOut, uint8_t *dataIn, unsigned int size);
    int Port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size);
    void Port_Delay_us(unsigned int us);
    void Port_Delay_ms(unsigned int ms);
    void Port_Error(void);
    
private:

};

#endif /* NRF24L01PPORTDRIVER_H */

