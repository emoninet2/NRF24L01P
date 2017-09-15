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

typedef enum{
     __NRF24L01p_GPIO_CSN,
     __NRF24L01p_GPIO_CE,
     __NRF24L01p_GPIO_IRQ,
 }__NRF24L01pGPIO_t;

class NRF24L01pPort {
public:
    NRF24L01pPort();
    NRF24L01pPort(const NRF24L01pPort& orig);
    virtual ~NRF24L01pPort();
    
    /**
     * initializes the low level drivers of the platform
     */
    void port_Initialize();
    /**
     * deinitializes the low level drivers of the platform
     */
    void port_DeInitialize();
    
     /**
     * initialization of the CE pin configuration
     */
    void port_Pin_CE_Initialize();
    /**
     * initialization of the CSN pin configuration
     */
    void port_Pin_CSN_Initialize();
    /**
     * initialization of the IRQ pin configuration
     */
    void port_Pin_IRQ_Initialize();
    
    /**
     * reads the logical value on the CE pin
     * return the logical value on CE pin
     */
    bool port_Pin_CE();
    /**
     * reads the logical value on the CSN pin
     * return the logical value on CSN pin
     */
    bool port_Pin_CSN();
    
    /**
     * sets the logical value on the CE pin
     * @param val the logical value to be set on the CE pin
     */
    void port_Pin_CE(bool val);
    /**
     * sets the logical value on the CSN pin
     * @param val the logical value to be set on the CSN pin
     */
    void port_Pin_CSN(bool val);
    
    
    /**
     * initializes the SPI configuration for the platform
     */
    void port_SPI_initialize();
    /**
     * exchange data on the SPI bus
     * @param dataOut pointer to the data buffer to read from the SPI
     * @param dataIn pointer the the data buffer to send data over SPI
     * @param size the number of bytes to be exchanged
     * @return returns the error status. 0 for success and -1 for fail
     */
    int port_SPI_Transcieve(uint8_t *dataOut, uint8_t *dataIn, unsigned int size);
    /**
     * exchange data on the SPI bus
     * @param dataInOut pointer to the data buffer to send data from and write back the exchanged data
     * @param size the number of bytes to be exchanged
     * @return returns the error status. 0 for success and -1 for fail
     */
    int port_SPI_Transcieve(uint8_t *dataInOut, unsigned int size);
    /**
     * Millisecond delay function supported by the platform
     * @param ms time value in milliseconds
     */
    void port_DelayMs(unsigned int ms);
    /**
     * Microsecond delay function supported by the platform
     * @param us time value in microseconds
     */
    void port_DelayUs(unsigned int us);
    
     /**
     * the global clock time elapsed in milliseconds
     * @return the global clock time elapsed since start in milliseconds
     */
    unsigned int port_ClockMs();
    
    
    
    
    
    ///The functions below are similar to the ones before and yet to be implemented in future
    
    
    void __NRF24L01p_port_initialize(void);
    void __NRF24L01p_port_deinitialize(void);

    void __NRF24L01p_port_GPIO_initialize(__NRF24L01pGPIO_t pin);
    void __NRF24L01p_port_GPIO_deinitialize(__NRF24L01pGPIO_t pin);
    bool __NRF24L01p_port_GPIO_read(__NRF24L01pGPIO_t pin);
    void __NRF24L01p_port_GPIO_write(__NRF24L01pGPIO_t pin, bool val);

    void __NRF24L01p_port_SPI_initialize(void);
    void __NRF24L01p_port_SPI_deinitialize(void);
    void __NRF24L01p_port_SPI_Transceive(uint8_t *data, unsigned int size);

    void __NRF24L01p_port_DelayMs(unsigned int ms);
    void __NRF24L01p_port_DelayUs(unsigned int us);
    unsigned int __NRF24L01p_port_ClockMS(void);

    
    
    
    
private:

};

#endif /* NRF24L01PPORT_H */

