/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pPort.h
 * Author: emon1
 *
 * Created on January 29, 2017, 8:03 AM
 */

#ifndef NRF24L01PPORT_H
#define NRF24L01PPORT_H

#include <stdint.h>

class NRF24L01pPort {
public:
    NRF24L01pPort();
    NRF24L01pPort(const NRF24L01pPort& orig);
    virtual ~NRF24L01pPort();
    
    void port_Initialize();
    void port_DeInitialize();
    void port_Pin_CE(bool val);
    void port_Pin_CSN(bool val);
    int port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size);
    void port_DelayMs(unsigned int ms);
    void port_DelayUs(unsigned int us);
    unsigned int port_ClockMs();
    
private:

};

#endif /* NRF24L01PPORT_H */

