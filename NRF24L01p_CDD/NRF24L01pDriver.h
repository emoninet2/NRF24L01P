/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pDriver.h
 * Author: emon
 *
 * Created on June 15, 2017, 3:52 PM
 */

#ifndef NRF24L01PDRIVER_H
#define NRF24L01PDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LKMH/gpio.h"
#include "LKMH/spi.h"
#include "NRF24L01pDefines.h"


#define __NRF24L01p_CSN_pin 7
#define __NRF24L01p_CE_pin 12


#define __NRF24L01p_SPI 1
#define __NRF24L01p_SPI_SPEED 500000

typedef enum {
      NRF24L01p_CONFIG_CRC_NONE      =  (0),
      NRF24L01p_CONFIG_CRC_8BIT      =  (_NRF24L01P_CONFIG_EN_CRC),
      NRF24L01p_CONFIG_CRC_16BIT     =  (_NRF24L01P_CONFIG_EN_CRC|_NRF24L01P_CONFIG_CRC0),
}NRF24L01p_CRC_t;

typedef enum {
      NRF24L01p_SETUP_AW_AW_3BYTE   =  (0x1<<0),/**< 3 bytes address width */
      NRF24L01p_SETUP_AW_AW_4BYTE   =  (0x2<<0),/**< 4 bytes address width */
      NRF24L01p_SETUP_AW_AW_5BYTE   =  (0x3<<0),/**< 5 bytes address width */
}NRF24L01p_AW_t;

typedef enum {
      NRF24L01p_RF_SETUP_RF_PWR_0DBM        =  (0x3<<1),
      NRF24L01p_RF_SETUP_RF_PWR_MINUS_6DBM  =  (0x2<<1),
      NRF24L01p_RF_SETUP_RF_PWR_MINUS_12DBM =  (0x1<<1),
      NRF24L01p_RF_SETUP_RF_PWR_MINUS_18DBM =  (0x0<<1),
}NRF24L01p_RFpower_t;

typedef enum {
      NRF24L01p_RF_SETUP_RF_DR_250KBPS    =    (_NRF24L01P_RF_SETUP_RF_DR_LOW_BIT),//(1 << 5)
      NRF24L01p_RF_SETUP_RF_DR_1MBPS      =    (0),
      NRF24L01p_RF_SETUP_RF_DR_2MBPS      =    (_NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT),//(1 << 3)
}NRF24L01p_datarate_t;

typedef enum {
      NRF24L01p_PIPE_P0       =    0,/**< Pipe 0 */
      NRF24L01p_PIPE_P1       =    1,/**< Pipe 1 */
      NRF24L01p_PIPE_P2       =    2,/**< Pipe 2 */
      NRF24L01p_PIPE_P3       =    3,/**< Pipe 3 */
      NRF24L01p_PIPE_P4       =    4,/**< Pipe 4 */
      NRF24L01p_PIPE_P5       =    5,/**< Pipe 5 */
}NRF24L01p_pipe_t;

typedef enum {
       NRF24L01p_MODE_UNKNOWN = 0,/**< NRF24L01+ unknown mode */
       NRF24L01p_MODE_POWER_DOWN,/**< NRF24L01+ Power Down mode */
       NRF24L01p_MODE_STANDBY,/**< NRF24L01+ Standby mode */
       NRF24L01p_MODE_RX,/**< NRF24L01+ RX mode */
       NRF24L01p_MODE_TX,/**< NRF24L01+ TX mode */
}NRF24L01p_RadioState_t;

typedef enum {
      NRF24L01p_FLAG_DATAREADY,
      NRF24L01p_FLAG_DATASENT,
      NRF24L01p_FLAG_MAXRETRY
}NRF24L01p_Flags_t;

typedef struct{
    bool PipeEn;
    bool autoAckEn;
    uint64_t PipeAddr;
    unsigned int PipeWidth;
    bool dynPldEn;
}NRF24L01p_RxPipeConfig_t;

typedef struct {
    bool DataReadyInterruptEnabled;
    bool DataSentInterruptFlagEnabled;
    bool MaxRetryInterruptFlagEnabled;
    NRF24L01p_CRC_t Crc;
    uint8_t AutoReTransmissionCount;
    uint8_t AutoReTransmitDelayX250us;
    uint8_t frequencyOffset;
    NRF24L01p_datarate_t datarate;
    NRF24L01p_RFpower_t RfPower;
    bool PllLock;
    bool ContWaveEnabled;
    bool FeatureDynamicPayloadEnabled;
    bool FeaturePayloadWithAckEnabled;
    bool FeatureDynamicPayloadWithNoAckEnabled;
    NRF24L01p_RxPipeConfig_t RxPipeConfig[6];
    uint64_t TxAddr;
}NRF24L01p_RadioSetting_t;


typedef enum{
        __NRF24L01p_GPIO_CSN,
        __NRF24L01p_GPIO_CE,
        __NRF24L01p_GPIO_IRQ,
    }__NRF24L01pGPIO_t;



#ifdef __cplusplus
}
#endif

#endif /* NRF24L01PDRIVER_H */

