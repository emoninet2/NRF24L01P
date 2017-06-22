#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include "NRF24L01p/NRF24L01p.h"


NRF24L01p Radio;
NRF24L01p::RadioConfig_t RadioConfig;
NRF24L01p::RxPipeConfig_t RxPipeConfig[6];

#define DEBUG_LEVEL 1


#define Debug(lvl, msg, args...) printf(msg, ##args)

#define DebugAssert(lvl, msg, args...) 
#if (lvl >= DEBUG_LEVEL)
Debug(msg, ##args)
#endif



void error(const char *msg);
void NRF24L01p_RadioReset();
bool charIsDelim(char c, char *delim);
int parseString(char * s, int len,  char * delim, int maxTokens, char ** argv);
int processRadioCommand(char *buffer, int size);


void NRF24L01p_RadioReset(){

    Radio.RadioConfig.DataReadyInterruptEnabled = 1;
    Radio.RadioConfig.DataSentInterruptEnabled = 1;
    Radio.RadioConfig.MaxRetryInterruptEnabled = 1;
    Radio.RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    Radio.RadioConfig.AutoReTransmissionCount = 15;
    Radio.RadioConfig.AutoReTransmitDelayX250us = 15;
    Radio.RadioConfig.frequencyOffset = 2;
    Radio.RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    Radio.RadioConfig.RfPower = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    Radio.RadioConfig.PllLock = 0;
    Radio.RadioConfig.ContWaveEnabled = 0;
    Radio.RadioConfig.FeatureDynamicPayloadEnabled = 1;
    Radio.RadioConfig.FeaturePayloadWithAckEnabled = 1;
    Radio.RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;

    Radio.RxPipeConfig[0].address = 0xAABBCCDDEE;
    Radio.RxPipeConfig[1].address = 0x6565656501;
    Radio.RxPipeConfig[2].address = 0x6565656502;
    Radio.RxPipeConfig[3].address = 0x6565656503;
    Radio.RxPipeConfig[4].address = 0x6565656509;
    Radio.RxPipeConfig[5].address = 0x6565656505;
 
    int i;
    for(i=0;i<6;i++){
        Radio.RxPipeConfig[i].PipeEnabled = 1;
        Radio.RxPipeConfig[i].autoAckEnabled = 1;
        Radio.RxPipeConfig[i].dynamicPayloadEnabled = 1;
    }
    
    //Radio.Initialize(&RadioConfig, RxPipeConfig);

    Radio.Initialize();
}


bool charIsDelim(char c, char *delim){

    char * pch = delim;
    pch=strchr(delim,c);
    
    if((pch-delim)>=0){
        return 1;
    }else{
        return 0;
    }
}


int parseString(char * s, int len,  char * delim, int maxTokens, char ** argv){
    int argc ;
       
    int i;
    bool removeWhiteSpace = 1;
    for(i=0;i<len;i++){

        if(  charIsDelim(s[i],delim)   && (removeWhiteSpace)  ){
            s[i] = 0;
        }

        if(s[i] == '"' ){
            s[i] = 0;
            removeWhiteSpace = !removeWhiteSpace;
        }
        
    }
    
    int start = 0;
    argc = 0;
    for(i=0;i<len;i++){
        if(s[i] != 0){
            start = i;
            argv[argc++] = &s[i];
            break;
        }
    }
    
    for(i=start+1;i<len;i++){
        if( (s[i] != 0) && (s[i-1] == 0)  ){
            argv[argc++] = &s[i];
            if(argc > maxTokens){
                break;
            }
        }
    }
    

    return argc;
}


int processRadioCommand(char *buffer, int size){
    
    char *tokenV[10];
    int tokenC = parseString(buffer, size,  " -\r\n" , 10, tokenV);
    int i;
    for(i=0;i<tokenC;i++){
        printf("token %d --> %s\r\n", i, tokenV[i]);
    }

    if(!strcmp(tokenV[0], "init")){
        NRF24L01p_RadioReset();
        *buffer = '\0';
    }
    else if(!strcmp(tokenV[0], "reset")){
        Radio.ReInitialize();
        *buffer = '\0';
    }
    else if(!strcmp(tokenV[0], "modeGet")){
        NRF24L01p::RadioState_t mode = Radio.RadioMode();
        switch(mode){
            case NRF24L01p::MODE_UNKNOWN : {
                sprintf(buffer,"%d unknown\r\n", mode);
            }break;
            case NRF24L01p::MODE_POWER_DOWN : {
                sprintf(buffer,"%d pwdn\r\n", mode);
            }break;
            case NRF24L01p::MODE_STANDBY : {
                sprintf(buffer,"%d stby\r\n", mode);
            }break;
            case NRF24L01p::MODE_RX : {
                sprintf(buffer,"%d rx\r\n", mode);
            }break;
            case NRF24L01p::MODE_TX : {
                sprintf(buffer,"%d tx\r\n", mode);
            }break;
            default:{
                sprintf(buffer,"-1 error\r\n");
            }break;     
        }

    }
    else if(!strcmp(tokenV[0], "modeSet")){
        NRF24L01p::RadioState_t mode;
       
        if( (!strcmp(tokenV[1], "pwdn")) ||  (!strcmp(tokenV[1], "1"))  ){
            mode = NRF24L01p::MODE_POWER_DOWN;
        }
        else if((!strcmp(tokenV[1], "stby")) ||  (!strcmp(tokenV[1], "2"))  ){
            mode = NRF24L01p::MODE_POWER_DOWN;
        }
        else if((!strcmp(tokenV[1], "rx")) ||  (!strcmp(tokenV[1], "3"))  ){
            mode = NRF24L01p::MODE_POWER_DOWN;
        }
        else if((!strcmp(tokenV[1], "tx")) ||  (!strcmp(tokenV[1], "4"))  ){
            mode = NRF24L01p::MODE_POWER_DOWN;
        }
        Radio.RadioMode(mode);
        *buffer = '\0';
    }
    else if(!strcmp(tokenV[0], "configWrite")){
        if(!strcmp(tokenV[1], "imsk")){
            int en = atoi(tokenV[3]);
            if(!strcmp(tokenV[2], "dr")){
                Radio.RadioConfig.DataReadyInterruptEnabled = en;
                Radio.dataReadyInterruptMask(en);
            }
            else if(!strcmp(tokenV[2], "ds")){
                Radio.RadioConfig.DataSentInterruptEnabled = en;
                Radio.dataSentInterruptMask(en);
            }
            else if(!strcmp(tokenV[2], "mr")){
                Radio.RadioConfig.DataSentInterruptEnabled = en;
                Radio.maxRetryInterruptMask(en);
            }
            *buffer = '\0';
        }
        else if(!strcmp(tokenV[1], "crc")){
            int opt = atoi(tokenV[2]);

            switch(opt){
                case 0 : {
                    Radio.RadioConfig.Crc = NRF24L01p::CONFIG_CRC_NONE;
                }break;
                case 8: {
                    Radio.RadioConfig.Crc = NRF24L01p::CONFIG_CRC_8BIT;
                }break;
                case 16: {
                    Radio.RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
                }break;
                default:{
                    sprintf(buffer,"-1 error\r\n");
                    return 0;
                }break;

            }
            Radio.CRC(Radio.RadioConfig.Crc);
            *buffer = '\0';
 
        }
        else if(!strcmp(tokenV[1], "aw")){
            int aw = atoi(tokenV[2]);
            
            if( aw ==3){
                Radio.RadioConfig.addressWidth = NRF24L01pDriver::SETUP_AW_AW_3BYTE ;
                Radio.addressWidth(NRF24L01pDriver::SETUP_AW_AW_3BYTE );
            }
            else if( aw ==4){
                Radio.RadioConfig.addressWidth = NRF24L01pDriver::SETUP_AW_AW_4BYTE ;
                Radio.addressWidth(NRF24L01pDriver::SETUP_AW_AW_4BYTE );
            }
            else if( aw ==5){
                Radio.RadioConfig.addressWidth = NRF24L01pDriver::SETUP_AW_AW_5BYTE ;
                Radio.addressWidth(NRF24L01pDriver::SETUP_AW_AW_5BYTE );
            }

            *buffer = '\0';
        }
        else if(!strcmp(tokenV[1], "arc")){
            int arc = atoi(tokenV[2]);
            
            if((arc >=0)&&(arc <= 15)){
                Radio.RadioConfig.AutoReTransmissionCount = arc;
                Radio.maxAutoRetransmissionCount(Radio.RadioConfig.AutoReTransmissionCount);
            }

            *buffer = '\0';
        }
        else if(!strcmp(tokenV[1], "ard")){
            int ardx250us =  (atoi(tokenV[2])/250) - 1 ;
            
            if((ardx250us >=0)&&(ardx250us <= 15)){
                Radio.RadioConfig.AutoReTransmitDelayX250us = ardx250us;
                Radio.autoRetransmissionDelay(ardx250us);
            }
            
            *buffer = '\0';
            
        }
        else if(!strcmp(tokenV[1], "freq")){
            int freq = atoi(tokenV[2]);
            int freq_off = freq - 2400;
            
            Radio.RadioConfig.frequencyOffset = freq_off;
            Radio.freqOffset(Radio.RadioConfig.frequencyOffset );
            
            *buffer = '\0';
        }
        else if(!strcmp(tokenV[1], "rfp")){
            
        }
        else if(!strcmp(tokenV[1], "pl")){
            
        }      
        else if(!strcmp(tokenV[1], "feat")){
            int en = atoi(tokenV[3]);
            if(!strcmp(tokenV[2], "dp")){
                Radio.RadioConfig.FeatureDynamicPayloadEnabled = en;
                Radio.dynamicPayloadFeature(en);
            }  
            else if(!strcmp(tokenV[2], "pwa")){
                Radio.RadioConfig.FeaturePayloadWithAckEnabled = en;
                Radio.payloadWithAckFeature(en);
            }  
            else if(!strcmp(tokenV[2], "dpna")){
                Radio.RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = en;
                Radio.dynamicPayloadWithNoAck(en);
            }  
        }  
        else if(!strcmp(tokenV[1], "rxpipe")){
            int pipe = atoi(tokenV[2]);
            
            if(!strcmp(tokenV[3], "en")){
                bool en = atoi(tokenV[4]);
                Radio.RxPipeConfig[pipe].PipeEnabled = en;
                Radio.RxOnPipe((NRF24L01p::pipe_t)pipe,en);
            }
            else if(!strcmp(tokenV[3], "ack")){
                bool en = atoi(tokenV[4]);
                Radio.RxPipeConfig[pipe].autoAckEnabled = en;
                Radio.autoAckOnPipe((NRF24L01p::pipe_t)pipe,en);
            }
            else if(!strcmp(tokenV[3], "wid")){
                int wid = atoi(tokenV[4]);
                Radio.RxPipeConfig[pipe].MaxWidth = wid;
                Radio.rxPipeWidth( (NRF24L01p::pipe_t)pipe,   wid);
            }
            else if(!strcmp(tokenV[3], "dynp")){
                bool en = atoi(tokenV[4]);
                Radio.RxPipeConfig[pipe].dynamicPayloadEnabled = en;
                Radio.dynamicPayloadOnPipe( (NRF24L01p::pipe_t)pipe, en);
            }
        } 
    }
    else if(!strcmp(tokenV[0], "configRead")){
        if(!strcmp(tokenV[1], "intMsk")){
            if(!strcmp(tokenV[2], "dr")){
                bool en = Radio.dataReadyInterruptMask();
                sprintf(buffer,"%d none\r\n", en);
            }
            else if(!strcmp(tokenV[2], "ds")){
                bool en = Radio.dataSentInterruptMask();
                sprintf(buffer,"%d none\r\n", en);                
            }
            else if(!strcmp(tokenV[2], "mr")){
                bool en = Radio.maxRetryInterruptMask();
                sprintf(buffer,"%d none\r\n", en);                
            }
        }
        else if(!strcmp(tokenV[1], "crc")){
            NRF24L01p::crc_t crc = Radio.CRC();
            switch(crc){
                case NRF24L01p::CONFIG_CRC_NONE : {
                    sprintf(buffer,"%d none\r\n", 0);
                }break;
                case NRF24L01p::CONFIG_CRC_8BIT : {
                    sprintf(buffer,"%d 8-bit\r\n", 8);
                }break;
                case NRF24L01p::CONFIG_CRC_16BIT : {
                    sprintf(buffer,"%d 16-bit\r\n", 16);
                }break;
                default : {
                    sprintf(buffer,"-1 error\r\n");
                }break;
            }
        }
        else if(!strcmp(tokenV[1], "aw")){
            NRF24L01p::aw_t aw = Radio.addressWidth();
            switch(aw){
                case NRF24L01p::SETUP_AW_AW_3BYTE :{
                    sprintf(buffer,"3 bytes\r\n");
                }break;
                case NRF24L01p::SETUP_AW_AW_4BYTE :{
                    sprintf(buffer,"4 bytes\r\n");
                }break;
                case NRF24L01p::SETUP_AW_AW_5BYTE :{
                    sprintf(buffer,"5 bytes\r\n");
                }break;   
                default : {
                    sprintf(buffer,"-1 error\r\n");
                }break;
            }
        }   
        else if(!strcmp(tokenV[1], "arc")){
            uint8_t arc = Radio.maxAutoRetransmissionCount();
            sprintf(buffer,"%d\r\n", arc);
        }
        else if(!strcmp(tokenV[1], "ard")){
            uint8_t ardx250us = Radio.autoRetransmissionDelay();
            sprintf(buffer, "%d us\r\n", (ardx250us + 1)*250);
        }
        else if(!strcmp(tokenV[1], "freq")){
            int freq_off = Radio.freqOffset();
            sprintf(buffer,"%d MHz\r\n", freq_off + 2400);
        }
        else if(!strcmp(tokenV[1], "rfp")){
            NRF24L01p::RFpower_t rfp = Radio.RfPower();
            switch(rfp){
                case NRF24L01p::RF_SETUP_RF_PWR_0DBM : {
                    sprintf(buffer,"0 dB\r\n");
                }break;
                case NRF24L01p::RF_SETUP_RF_PWR_MINUS_6DBM : {
                    sprintf(buffer,"-6 dB\r\n");
                }break;
                case NRF24L01p::RF_SETUP_RF_PWR_MINUS_12DBM : {
                    sprintf(buffer,"-12 dB\r\n");
                }break;
                case NRF24L01p::RF_SETUP_RF_PWR_MINUS_18DBM : {
                    sprintf(buffer,"-18 dB\r\n");
                }break;     
                default : {
                    sprintf(buffer,"-1 error\r\n");
                }break;
            }
        }
        else if(!strcmp(tokenV[1], "pl")){
            bool pl = Radio.pllLock();
            sprintf(buffer,"%d\r\n", pl);
        }    
        else if(!strcmp(tokenV[1], "cw")){
            bool cw = Radio.contWave();
            sprintf(buffer,"%d\r\n", cw);
        }   
        else if(!strcmp(tokenV[1], "feat")){
            if(!strcmp(tokenV[2], "dp")){
                bool dp = Radio.dynamicPayloadFeature();
                sprintf(buffer,"%d\r\n", dp);
            }  
            else if(!strcmp(tokenV[2], "pwa")){
                bool pwa = Radio.payloadWithAckFeature();
                sprintf(buffer,"%d\r\n", pwa);
            }  
            else if(!strcmp(tokenV[2], "dpna")){
                bool dpna = Radio.dynamicPayloadWithNoAck();
                sprintf(buffer,"%d\r\n", dpna);
            }  
        }  
        else if(!strcmp(tokenV[1], "rxpipe")){
            int pipe = atoi(tokenV[2]);
            if(!strcmp(tokenV[3], "en")){
                bool en = Radio.RxOnPipe((NRF24L01p::pipe_t)pipe);
                sprintf(buffer,"%d\r\n", en);
            }
            else if(!strcmp(tokenV[3], "ack")){
                bool en = Radio.autoAckOnPipe((NRF24L01p::pipe_t)pipe);
                sprintf(buffer,"%d\r\n", en);
            }
            else if(!strcmp(tokenV[3], "wid")){
                uint8_t wid = Radio.rxPipeWidth((NRF24L01p::pipe_t)pipe);
                sprintf(buffer,"%d\r\n", wid);
            }
            else if(!strcmp(tokenV[3], "dynp")){
                bool en = Radio.dynamicPayloadOnPipe((NRF24L01p::pipe_t)pipe);
                sprintf(buffer,"%d\r\n", en);
            }
        } 
    }
    else if(!strcmp(tokenV[0], "addrWrite")){
        uint64_t addr;
        
        if(!strcmp(tokenV[1], "tx")){
            Radio.TxPipeAddress = (NRF24L01p::PipeAddr_t) strtoll(tokenV[2], NULL, 16);
            Radio.set_TX_pipe_address(Radio.TxPipeAddress);
        }
        if(!strcmp(tokenV[1], "rx")){
            int pipe = atoi(tokenV[2]);
            Radio.RxPipeConfig[pipe].address = (NRF24L01p::PipeAddr_t)strtoll(tokenV[3], NULL, 16);
            addr = strtoll(tokenV[3], NULL, 16);
            //Radio.set_RX_pipe_address((NRF24L01p::pipe_t) pipe,addr);
            Radio.set_RX_pipe_address((NRF24L01p::pipe_t) pipe , (Radio.RxPipeConfig[pipe].address));

        }
    }
    else if(!strcmp(tokenV[0], "addrRead")){
        uint64_t addr;
        if(!strcmp(tokenV[1], "tx")){
            addr = Radio.get_TX_pipe_address();
            sprintf(buffer,"%#llx\r\n", addr);
        }
        else if(!strcmp(tokenV[1], "rx")){
            int pipe = atoi(tokenV[2]);
            addr = Radio.get_RX_pipe_address((NRF24L01p::pipe_t) pipe);
            sprintf(buffer,"%#llx\r\n", addr);
            printf("pipe %d addr is %llx\r\n", pipe, addr);
        }
    }
    else if(!strcmp(tokenV[0], "flagRead")){
        bool flag;
        if(!strcmp(tokenV[1], "dr")){
            flag = Radio.DataReadyFlag();
        }
        else if(!strcmp(tokenV[1], "ds")){
            flag = Radio.DataSentFlag();
        }
        else if(!strcmp(tokenV[1], "mr")){
            flag = Radio.MaxRetryFlag();
        }
        
        sprintf(buffer,"%d\r\n", flag);
    }
    else if(!strcmp(tokenV[0], "flagClear")){
        if(!strcmp(tokenV[1], "dr")){
            Radio.clearDataReadyFlag();
        }
        else if(!strcmp(tokenV[1], "ds")){
            Radio.clearDataSentFlag();
        }
        else if(!strcmp(tokenV[1], "mr")){
            Radio.clearMaxRetryFlag();
        }
        *buffer = '\0';
    }
    
    
    else if(!strcmp(tokenV[0], "status")){
        uint8_t status = Radio.get_status();
        sprintf(buffer,"%#x\r\n", status);
    }
    else if(!strcmp(tokenV[0], "readable")){
        int retval = Radio.readable();
        sprintf(buffer,"%d\r\n", retval);
    }
    else if(!strcmp(tokenV[0], "writable")){
        int retval = Radio.writable();
        sprintf(buffer,"%d\r\n", retval);
    }
    else if(!strcmp(tokenV[0], "rx")){
        uint64_t addr;
        NRF24L01p::Payload_t payload;
        int retval = Radio.ReceivePayload(&payload);
        sprintf(buffer,"%d %d %d %s\r\n", retval , payload.pipe, payload.length, payload.data) ;
        
    }
    else if(!strcmp(tokenV[0], "tx")){
        uint64_t addr;
        NRF24L01p::Payload_t payload;
        payload.address = strtoll(tokenV[1], NULL, 16);
        payload.retransmitCount = atoi (tokenV[2]);
        payload.UseAck = atoi(tokenV[3]);
        payload.length = atoi(tokenV[4]);
        memcpy(payload.data, tokenV[5], payload.length);
        
        int retval = Radio.TransmitPayload(&payload);
        if(payload.UseAck == 0) sprintf(buffer,"%d\r\n", retval);
        else sprintf(buffer,"%d %d %s\r\n", retval , payload.length, payload.data);
        
    }
    else if(!strcmp(tokenV[0], "flushTx")){
        Radio.flush_tx();
        *buffer = '\0';
    }
    else if(!strcmp(tokenV[0], "flushRx")){
        Radio.flush_rx();
        *buffer = '\0';
    }
    else if(!strcmp(tokenV[0], "exit")){
        *buffer = '\0';
        sprintf(buffer,"exiting program\r\n");
        return 1;
    }
    else if(!strcmp(tokenV[0], "info")){
        
    }
    else{
        sprintf(buffer,"-1 \"unknown command\"\r\n");
    }
    
    
    return 0;
}



int main(int argc, char** argv)
{


    
    Debug(0, "starting NRF24L01p server\r\n");
    //debug(5, "thats the way i like it\r\n");
    //return 0;
    
    while(1){
            int sockfd, newsockfd, portno;
             socklen_t clilen;
             char buffer[1024];
             struct sockaddr_in serv_addr, cli_addr;
             int n;
             if (argc < 2) {
                 fprintf(stderr,"ERROR, no port provided\n");
                 exit(1);
             }
             sockfd = socket(AF_INET, SOCK_STREAM, 0);
             if (sockfd < 0) 
                error("ERROR opening socket");
             bzero((char *) &serv_addr, sizeof(serv_addr));
             portno = atoi(argv[1]);
             serv_addr.sin_family = AF_INET;
             serv_addr.sin_addr.s_addr = INADDR_ANY;
             serv_addr.sin_port = htons(portno);

            if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");


            while(1){
                listen(sockfd,5);
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, 
                            (struct sockaddr *) &cli_addr, 
                            &clilen);
                printf("connected\r\n");
                if (newsockfd < 0) 
                    break;
                bzero(buffer,1024);
                while(1){
                    n = read(newsockfd,buffer,255);
                    if (n <= 0) break;
                    
                    *buffer = '\0';
                    printf("\tmessage[%d]: %s\r\n",n, buffer);
 
                    int cmd = processRadioCommand(buffer, n);
                    
                    if(cmd == 1){
                        Debug(0, "exiting server\r\n");
                        close(newsockfd);
                        close(sockfd);
                        return 0;
                    }
                                  
                    n = write(newsockfd,buffer,strlen(buffer));
                    if (n < 0) break;

                }

                
                printf("client connection closed\r\n");
             }

            close(newsockfd);
            close(sockfd);
        }

    return 0; 
}





void error(const char *msg)
{
    perror(msg);
    exit(1);
}