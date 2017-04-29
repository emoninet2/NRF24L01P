#include <stdio.h> 
#include <argp.h> 
#include <stdlib.h>
#include <stdint.h>
#include <argz.h>

#include "NRF24L01p/NRF24L01p.h"


NRF24L01p Radio;
NRF24L01p::RadioConfig_t RadioConfig;
NRF24L01p::RxPipeConfig_t RxPipeConfig[6];

void NRF24L01p_RadioReset(){
    RadioConfig.DataReadyInterruptEnabled = 1;
    RadioConfig.DataSentInterruptFlagEnabled = 1;
    RadioConfig.MaxRetryInterruptFlagEnabled = 1;
    RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPower = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;

    RxPipeConfig[0].address = 0xAABBCCDDEE;
    RxPipeConfig[1].address = 0x6565656501;
    RxPipeConfig[2].address = 0x6565656502;
    RxPipeConfig[3].address = 0x6565656503;
    RxPipeConfig[4].address = 0x6565656509;
    RxPipeConfig[5].address = 0x6565656505;
 
    int i;
    for(i=0;i<6;i++){
        RxPipeConfig[i].PipeEnabled = 1;
        RxPipeConfig[i].autoAckEnabled = 1;
        RxPipeConfig[i].dynamicPayloadEnabled = 1;
    }
    
    Radio.Initialize(&RadioConfig, RxPipeConfig);

}

struct arguments {   
    char *argz;   
    size_t argz_len; 
    int addr;
    int length;
    uint8_t *data;
    NRF24L01p::pipe_t pipe;
    NRF24L01p::PipeAddr_t pipeAddress;
    NRF24L01p::Payload_t payload;
};


static int parse_opt (int key, char *arg, struct argp_state *state) 
{   
    //int *arg_count = (int*)state->input;
    struct arguments *a = (arguments*) state->input;
    
    
    switch (key)     {   
        case 990: {    
            NRF24L01p_RadioReset();
            break;
        }
        case 993: {   
            //parse_opt ('R', "7", state); 
            //parse_opt (999, 0, state); 
            printf("%#x\r\n", Radio.get_status());
            break;
        }
        case 899: {   
            a->addr = atoi(arg); 
            break;
        }
        case 999: {   

            break;
        }
        case 998: { 
            break;
        }
        case 997: { 
            uint8_t len;
            sscanf(arg,"%x", &len);
            uint8_t data[32];
            Radio.read_rx_payload(data, len);
            data[len] = 0;
            int i;
            //for(i=0;i<len;i++) printf("%s", data[i]);
            printf("%s\r\n", data);
            break;
        }
        case 996: { 
            printf("writing Tx Payload [%d], %s\r\n", a->length, arg);
            Radio.write_tx_payload((uint8_t*)arg, a->length);
            break;
        }
        case 898: { 
            a->length = atoi(arg);
            break;
        }
        case 995: {    
            Radio.flush_tx();
            break;
        }
        case 994: {    
            Radio.flush_rx();
            break;
        }
        case 992:{
            Radio.clear_max_retry_flag();
            break;
        }
        case 991:{
            printf("%#x\r\n", Radio.read_register(_NRF24L01P_REG_FIFO_STATUS));
            break;
        }
        case 897: {    
            a->data = (uint8_t*)arg;
            break;
        }
        case 896: { 
            int  pipe;
            sscanf(arg,"%d", &pipe);
            a->pipe = (NRF24L01p::pipe_t) pipe;  
            //printf("pipe : %d\r\n", a->pipe);
            break;
        }
        case 895: { 
            uint64_t  pipeAddr;
            printf("getting pipe addr\r\n");
            pipeAddr = Radio.get_TX_pipe_address();
            printf("%#llx\r\n", pipeAddr);
            break;

        }
        case 894: { 
            printf("setting Tx Addr\r\n");
            uint64_t  pipeAddr;
            sscanf(arg,"%llx", &pipeAddr);
            Radio.set_TX_pipe_address(pipeAddr);
            break;
        }
        case 893: { 
            printf("getting Tx Addr\r\n");
            uint64_t  pipeAddr;
            pipeAddr = Radio.get_RX_pipe_address(a->pipe);
            printf("%#llx\r\n", pipeAddr);
            break;
        }
        case 892: { 
        uint64_t  pipeAddr;
            sscanf(arg,"%llx", &pipeAddr);
            Radio.set_RX_pipe_address(a->pipe, pipeAddr);
            break;
        }

        case 850: { 
            printf("%d\r\n", (int)Radio.RadioMode());
            break;
        }
        case 851: { 
            NRF24L01p::RadioState_t mode = (NRF24L01p::RadioState_t) atoi(arg);
            Radio.RadioMode(mode);
            break;
        }
       
        
        case ARGP_KEY_ARG: {     
            argz_add (&a->argz, &a->argz_len, arg);     
            break;
        }       
        case ARGP_KEY_INIT: {
            a->argz = 0;
            a->argz_len = 0;
            break;
        }
        case ARGP_KEY_END: {    
            size_t count = argz_count (a->argz, a->argz_len); 
            //printf ("\r\nprogram ending \n");  
            break;
        }       
    }   
    return 0; 
} 


int main (int argc, char **argv) {   
    Radio.port_Initialize();
    
    struct argp_option options[] =     {    
        { 0, 0, 0, 0, "NRF24L01+:", 7},
        { "init", 990, 0, 0, "Initialize radio"}, 
        { "addr", 899, "ADDR", 0, "Register"}, 
        { "RegRead", 999, "ADDR", 0, "Read Register"}, 
        { "RegWrite", 998, "ADDR", 0, "Write Register"}, 
        { "readRxPayload", 997, 0, 0, "Read Rx Payload"}, 
        { "writeTxPayload", 996, "DATA", 0, "Write Rx Payload"}, 
        { "flushTx", 995, 0, 0, "Flush Tx Buffer"}, 
        { "flushRx", 994, 0, 0, "Flush Tx Buffer"}, 
        { "status", 993, 0, 0, "Status"}, 
        { "clear", 992, 0, 0, "Clear Flag"}, 
        { "fifoStatus", 991, 0, 0, "Fifo Status"}, 
        { "len", 898, "BYTE", 0, "Length value"}, 
        { "data", 897, "DATA", 0, "Data value"}, 
        { "pipe", 896, "PIPE", 0, "Data value"}, 
        { "getTxAddr", 895, 0, 0, "Data value"}, 
        { "setTxAddr", 894, "ADDR", 0, "Data value"}, 
        { "getRxAddr", 893, 0, 0, "Data value"}, 
        { "setRxAddr", 892, "ADDR", 0, "Data value"}, 
        { "getMode", 850, 0, 0, "[1:PWR_DOWN 2: STDBY 3: RX 4:TX]"}, 
        { "setMode", 851, "MODE", 0, "[1:PWR_DOWN 2: STDBY 3: RX 4:TX]"}, 
        
        
        { 0 }     
    };   
    
    struct argp argp = { options, parse_opt };  
    struct arguments arguments; 
    return argp_parse (&argp, argc, argv, 0, 0, &arguments); 
}