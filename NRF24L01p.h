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


    typedef enum {
           MODE_UNKNOWN = 0,/**< NRF24L01+ unknown mode */
           MODE_POWER_DOWN,/**< NRF24L01+ Power Down mode */
           MODE_STANDBY,/**< NRF24L01+ Standby mode */
           MODE_RX,/**< NRF24L01+ RX mode */
           MODE_TX,/**< NRF24L01+ TX mode */
    }RadioState_t;
    
     /** the structure of the Radio Configuration
      * This structure stores the default configuration assigned by the user
     */
    typedef struct {
        bool DataReadyInterruptEnabled; /**< setting this high will generate interrupt signal on IRQ pin if a new data has arrived */
        bool DataSentInterruptEnabled;  /**< setting this high will generate interrupt signal on IRQ pin if data has been sent successfully */
        bool MaxRetryInterruptEnabled;  /**< setting this high will generate interrupt signal on IRQ pin if data transmission failed and max retry has been reached*/
        crc_t Crc;                      /**< Choose if the payload crc has to be 8-bit or 16-bit. Default is 16-bit crc*/
        aw_t addressWidth;              /**< choose the address byte to be either 3, 4 or 5 bytes. Default is 5-bytes*/
        uint8_t AutoReTransmissionCount;/**< used in the EnhancedShockBurst to retransmit data automatically. Value range from 0 to 15*/
        uint8_t AutoReTransmitDelayX250us;/**< the number (0 to 15 ) times 250 uS is the waiting period before retransmitting the data on failure */
        uint8_t frequencyOffset;/**< frequency offset over 2400 mhz. Maximum value possible is 125, allowing frequency to shift until 2526Mhz*/
        datarate_t datarate;/**< the on air data rate can be either 250kbps, 1Mbps or 2Mbps*/
        RFpower_t RfPowerDb;/**< transmitting power of the radio can be either 0dBm, -6dBm, -12dBm, -18dBm. Default is 0dBm (maximum power)*/
        bool PllLock;/**< used during physical layer debugging when continuous carrier wave is generated*/
        bool ContWaveEnabled;/**< will generate a continuous carrier signal at the set frequency*/
        bool FeatureDynamicPayloadEnabled;/**< setting this high will allow the payload size to be dynamically set and read. */
        bool FeaturePayloadWithAckEnabled;/**< setting this high will allow the ack payload size to be dynamically set and read*/
        bool FeatureDynamicPayloadWithNoAckEnabled;/**< will allow transmission of a single payload with no ack request*/
    }RadioConfig_t;
    
    /**struucture of the 6 RxPipeConfig
     * Each individual pipe configurations such as to enable it, enable auto ack on the pipe the pipe address, maximum width and feature to allow dynamic payload
     */
    typedef struct {
        bool PipeEnabled;           /** < set this to high to allow data reception on the pipe */
        bool autoAckEnabled;        /** < set this high to automatically send an ack packet if a data has been received on the pipe*/
        uint64_t address;           /** < the 5-byte (can be 3, or 4) unique address of the pipe*/
        uint8_t MaxWidth;           /** < the maximum byte allowed to be received on the pipe. (only use if dynamic payload size feature is disabled)*/
        bool dynamicPayloadEnabled; /** < setting this high will allow payload size to be automatically determined */
    }RxPipeConfig_t;



    
    /** the structure of the Payload configuration
     * This structure is common for both payload to send and receive
     */
    typedef struct{
        
        /// to be deprecated
        pipe_t pipe; //if payload type is RX

        ///to be used
        uint8_t Data[_NRF24L01P_FIFO_SIZE ]; //*< this contains the 32 bytes buffer data to send and receive into */
        unsigned int length;//*< the length of the data to send  or received*/
        
        bool UseAck;//*< used in TX mode to send the payload with ack if set to high and without ack if set to low*/
        PipeAddr_t TxAddress;//*< this is the 5-byte hardware address of the radio to send data to*/
        pipe_t TxAckPipe;//*< used when an ack payload has to be written in RX mode. when a payload is received , the ack payload is automatically sent*/
        bool GotAck;//*< this value is set high if the transmitter receives and ack.*/
        pipe_t RxPipe; //*< the pipe in which the data the data has been received and read from*/
        //unsigned int RxDataLen;//*< the size of the data received. In Tx Mode, the size of the ack payload received*/
    }Payload_t;
    
    /**
     * The enum containing the types of possible errors
     */
    typedef enum{
        SUCCESS = 0, /**< succes operation */
        ERROR = -1,  /**< failed operation */ 
    }ErrorStatus_t;
    
    /** The variable to remember the current state of the radio
     * The statue of the radio initially is unknown. However changing states may require entering one more step which requires knowledge of the current state
     */
    RadioState_t RadioState;
    
    /** The placeholder for the radio's states defined by the user
     * This needs to be set one time before initializing the radio. The radio may know what to configure in cases of reset or other cases. 
     */
    RadioConfig_t RadioConfig;
    
    /* Placeholder for the RX pipe configurations
     * All the configurations of the 6 parallel pipes are registered in this placeholder
     */
    RxPipeConfig_t RxPipeConfig[6];
    
    /* Placeholder for the TX address
     * Usually the TX address changes every time depending on the address to send to. However this value can be used if user wants to register a default TX address
     */
    PipeAddr_t  TxPipeAddress;
    
    
    /* The flags status set by the IRQ interrupt handler
     * These variables are used when the radio uses the interrupt handler function. The radio may either block the process by method of polling (reading the status register) or checing these values set by the interrupt function
     */
    bool drFlag, dsFlag, mrFlag;
    
    NRF24L01p();
    NRF24L01p(const NRF24L01p& orig);
    virtual ~NRF24L01p();
    
    
    
    
    /* Initialize radio based on the user defined configurations
     * initializes the radio based on the configurations set in the placeholder RadioConfig
     */
    void Initialize();
    /* ReInitialize radio based on the user defined configurations
     * same as initialization. However some ports may create conflicts during low level re-init. 
     */
    void ReInitialize();
    
    /* reads the current status of the radio FSM
     * @return the state of the radio
     */
    RadioState_t RadioMode();
    /* sets the current status of the radio FSM
     * @param mode set the current mode as power down, standby, TX or RX. 
     */
    void RadioMode(RadioState_t mode);
    
    /**check if the radio is has new data to read
     * this value is set if there is new data readable. It is determined by the FIFO status of the radio
     * @return high if there is new data readable. low if there is no new data
     */
    bool readable();
    /**check if the radio has its TX FIFO free to write
     * the value if high means that new data can be written for transmission. low means the radio is busy
     * @return high if the data is writable. low means the radio is busy or has pending transmission of data
     */
    bool writable();
    /**Function to check if a pipe have received any data
     * This function will check the pipe address of the last in the RX fifo. if data is readable, this function will tell which pipe has received the data
     * @param pipe the pipe in which the data has been received (0 to 6)
     * @return returns true if there is pending data to be read on the pipe. false if there is no received data on the pipe. 
     */
    bool readableOnPipe(pipe_t pipe);
    
    /** write the payload to transmit
     * this will write the 32-byte max payload that needs to be transmitted. 
     * This function will check if the payload needs to be sent with ack and determine the sub function to send with ack or without
     * @param payload The payload containing the data and its configurations. 
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t writePayload(Payload_t *payload);
    
    /**write the ack payload
     * This function is used during reception mode. 
     * When a pipe receives a data, it may send an ack payload that can be empty or contain some data
     * This function will preload the data that will automatically be sent when data is arrived on that pipe
     * Set the pipe in the payload's member 'TxAckPipe'
     * This function does not need to be used by the user. It is used by the function TrnasmitPayload() and TransmitPayloadInterruptHandled()
     * @param payload the payload containing the data 
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t writeAckPayload(Payload_t *payload);
    
    /** reads data received in the radio
     * check if there is data to read by using the function readable() and then use this function to extract the data, data size and pipe
     * @param payload the payload the data has to be written into
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t readPayload(Payload_t *payload);
    
    /** transmit the payload on air
     * this function will transmit the payload
     * @param payload the payload containing the data and other configurations such as address, use of act, etc
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t TransmitPayload(Payload_t *payload);
    
    /** reads data received in the radio and flush the RX buffer on error
     * check if there is data to read by using the function readable() and then use this function to extract the data, data size and pipe
     * @param payload the payload the data has to be written into
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t ReceivePayload(Payload_t *payload);
    
#if (_NRF24L01P_INTERRUPT_FEATURE_API == 1)   
    /** transmit the payload on air (interrupt handled)
     * this function will transmit the payload
     * @param payload the payload containing the data and other configurations such as address, use of act, etc
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t TransmitPayloadInterruptHandled(Payload_t *payload);
    /** reads data received in the radio and flush the RX buffer on error (interrupt handled)
     * check if there is data to read by using the function readable() and then use this function to extract the data, data size and pipe
     * @param payload the payload the data has to be written into
     * @return returns the error status as success, or fail
     */
    ErrorStatus_t ReceivePayloadInterruptHandled(Payload_t *payload);
    
    /**
     * The function to handle the interrupt generated on the IRQ pin
     */
    void InterruptHandler(void);
#endif   

#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)   
    typedef struct{
        Payload_t *payload;
        unsigned int head;
        unsigned int tail;
        unsigned int size;
    }fifo_t;
    
    fifo_t TxFifo;
    fifo_t RxFifo;

    Payload_t RxFifoBuffer[_NRF24L01P_SOFTWARE_FIFO_SIZE];
    Payload_t TxFifoBuffer[_NRF24L01P_SOFTWARE_FIFO_SIZE];
    
    ErrorStatus_t fifo_init(fifo_t *f, Payload_t  *pld, unsigned int size);
    ErrorStatus_t fifo_read(fifo_t *f, Payload_t  *pld);
    ErrorStatus_t fifo_peek(fifo_t *f, Payload_t  *pld);
    ErrorStatus_t fifo_write(fifo_t *f, Payload_t  *pld);
    unsigned int fifo_waiting(fifo_t *f);
    unsigned int fifo_freeSpace(fifo_t *f);
    ErrorStatus_t fifo_reset(fifo_t *f);
    ErrorStatus_t TransmitPayloadViaFifo(Payload_t *payload);
    ErrorStatus_t ReceivePayloadViaFifo(Payload_t *payload);
    void process(void);
    void processInterruptHandled(void);
    
    
    
    ////////////////API under development//////////////
    ErrorStatus_t CarrierDetectAndTransmitPayload(Payload_t *payload);
    float TestCarrierQuality(unsigned int frequencyOffset, unsigned int readtimes);
    void GenerateCarrierQualityReport(unsigned int readtimes);

    
#endif
    
    void hardwareCheck();
    
private:

};

#endif /* NRF24L01P_H */

