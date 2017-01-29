/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01p.h
 * Author: emon1
 *
 * Created on January 29, 2017, 7:10 AM
 */

#ifndef NRF24L01P_H
#define NRF24L01P_H

#include "NRF24L01pDriver.h"
#include "NRF24L01pConfig.h"


class NRF24L01p : public NRF24L01pDriver{
public:
    NRF24L01p();
    NRF24L01p(const NRF24L01p& orig);
    virtual ~NRF24L01p();
    
    
        typedef struct{
        uint8_t data[32];
        uint64_t TxAddr;
        pipe_t RxPipe;
        uint8_t len;
    }Payload_t;
    
    typedef enum {
           MODE_UNKNOWN = 0,/**< NRF24L01+ unknown mode */
           MODE_POWER_DOWN,/**< NRF24L01+ Power Down mode */
           MODE_STANDBY,/**< NRF24L01+ Standby mode */
           MODE_RX,/**< NRF24L01+ RX mode */
           MODE_TX,/**< NRF24L01+ TX mode */
    } StateType;

    typedef struct{
        Payload_t *payload;
        unsigned int head;
        unsigned int tail;
        unsigned int size;
    }fifo_t;
    
    
    int fifo_init(fifo_t *f, Payload_t  *pld, unsigned int size);
    int fifo_read(fifo_t *f, Payload_t  *pld);
    int fifo_peekNext(fifo_t *f, Payload_t  *pld);
    int fifo_write(fifo_t *f, Payload_t  *pld);

    
    StateType RadioState;
    
    Payload_t RxFifoBuffer[NRF24L01P_FIFO_SIZE ];
    Payload_t TxFifoBuffer[NRF24L01P_FIFO_SIZE ];
    
    fifo_t TxFifo;
    fifo_t RxFifo;

    uint64_t TxAddr;

    void initialize();
    int startup();
    int default_config();
    void RadioMode(StateType mode);
    
    bool readable();
    bool writable();

    int write_payload_to_send(uint8_t *data, int datalen);
    int write_payload_to_send_to_address(pipeAddrType_t address, uint8_t *data, int datalen);
    int write_payload_to_send_to_address_ack(pipeAddrType_t address, uint8_t *data, int datalen);
    int write_payload_to_send_to_address_noack(pipeAddrType_t address, uint8_t *data, int datalen);
    bool readableOnPipe(pipe_t pipe);
    int read_payload(pipe_t pipe, uint8_t *data, int datalen);
    int read_payload_dyn(pipe_t pipe, uint8_t *data);
    void write_ack(pipe_t pipe, uint8_t *data, int datalen);

    int TransmitPacket(Payload_t *payload);
    int ReceivePacket(Payload_t *payload);
    
    
    void PRX();
    void PTX();
    
    
    
private:

};

#endif /* NRF24L01P_H */

