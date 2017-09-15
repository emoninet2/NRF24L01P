/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01p.cpp
 * Author: emon1
 * 
 * Created on January 29, 2017, 7:10 AM
 */

#include "NRF24L01p.h"




NRF24L01p::NRF24L01p() {
    
    RadioState = MODE_UNKNOWN;
    
    
    RadioConfig.DataReadyInterruptEnabled = 1;
    RadioConfig.DataSentInterruptEnabled = 1;
    RadioConfig.MaxRetryInterruptEnabled = 1;
    RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPowerDb = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;
    
    RxPipeConfig[0].address = 0xe7e7e7e7e7;
    RxPipeConfig[1].address = 0xc2c2c2c2c2;
    RxPipeConfig[2].address = 0xc2c2c2c2c3;
    RxPipeConfig[3].address = 0xc2c2c2c2c4;
    RxPipeConfig[4].address = 0xc2c2c2c2c5;
    RxPipeConfig[5].address = 0xc2c2c2c2c6;
    
}

NRF24L01p::NRF24L01p(const NRF24L01p& orig) {
}

NRF24L01p::~NRF24L01p() {
}

void NRF24L01p::ReInitialize(){
    port_Pin_CE(0);
    port_Pin_CSN(0);

    port_DelayMs(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clearDataReadyFlag();
    clearDataSentFlag();
    clearMaxRetryFlag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);
    
    dataReadyInterruptMask(RadioConfig.DataReadyInterruptEnabled);
    dataSentInterruptMask(RadioConfig.DataSentInterruptEnabled);
    maxRetryInterruptMask(RadioConfig.MaxRetryInterruptEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled);
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled);
    maxAutoRetransmissionCount(RadioConfig.AutoReTransmissionCount);
    autoRetransmissionDelay(RadioConfig.AutoReTransmitDelayX250us);        
    DataRate(RadioConfig.datarate);
    
    int i;
    for(i=0;i<6;i++){
        RxOnPipe((pipe_t)i,RxPipeConfig[i].PipeEnabled );
        autoAckOnPipe((pipe_t)i,RxPipeConfig[i].autoAckEnabled );
        dynamicPayloadOnPipe((pipe_t)i,RxPipeConfig[i].dynamicPayloadEnabled);
        rxPipeAddress((pipe_t)i,RxPipeConfig[i].address);
    }
#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)      
    fifo_init(&TxFifo, TxFifoBuffer, 10);
    fifo_init(&RxFifo, RxFifoBuffer, 10);
#endif
}
void NRF24L01p::Initialize(){

    port_Initialize();

    port_Pin_CE(0);
    port_Pin_CSN(0);

    port_DelayMs(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clear_data_ready_flag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);



    dataReadyInterruptMask(RadioConfig.DataReadyInterruptEnabled);
    dataSentInterruptMask(RadioConfig.DataSentInterruptEnabled);
    maxRetryInterruptMask(RadioConfig.MaxRetryInterruptEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled);
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled);
    maxAutoRetransmissionCount(RadioConfig.AutoReTransmissionCount);
    autoRetransmissionDelay(RadioConfig.AutoReTransmitDelayX250us);        
    DataRate(RadioConfig.datarate);
    freqOffset(RadioConfig.frequencyOffset);
    

    RfPower(RadioConfig.RfPowerDb);
    pllLock(RadioConfig.PllLock);
    contWave(RadioConfig.ContWaveEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled );
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled );
    
    
    int i;
    for(i=0;i<6;i++){
        RxOnPipe((pipe_t)i,RxPipeConfig[i].PipeEnabled );
        autoAckOnPipe((pipe_t)i,RxPipeConfig[i].autoAckEnabled );
        dynamicPayloadOnPipe((pipe_t)i,RxPipeConfig[i].dynamicPayloadEnabled);
        rxPipeAddress((pipe_t)i,RxPipeConfig[i].address);
    }
    
#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)  
    fifo_init(&TxFifo, TxFifoBuffer, 10);
    fifo_init(&RxFifo, RxFifoBuffer, 10);
#endif    
}

NRF24L01p::RadioState_t NRF24L01p::RadioMode(){
    bool _ce = port_Pin_CE();
    uint8_t _config = read_register(_NRF24L01P_REG_CONFIG)  ;
    bool _pwr = (_config>>1)&0x01;
    bool _txrx = (_config>>0)&0x01;


    if(_pwr == 0){
        return MODE_POWER_DOWN;
    }
    else{
        if(_ce == 0){
            return MODE_STANDBY;
        }
        else{
            if(_txrx==0){
                return MODE_TX;
            }else{
                return MODE_RX;
            }
        }
    } 
}

void NRF24L01p::RadioMode(NRF24L01p::RadioState_t mode){
    RadioState = RadioMode();
    
    switch(mode){
        case MODE_POWER_DOWN: {
            power_down();
            port_Pin_CE(0);
            RadioState = MODE_POWER_DOWN;
            break;
        }
        case MODE_STANDBY: {
            if(RadioState == MODE_POWER_DOWN){
                    power_up();
                    port_DelayUs(_NRF24L01P_TIMING_Tpd2stby_us);
            }
            else{
                    port_Pin_CE(0);
            }
            RadioState = MODE_STANDBY;
            break;
        }
        case MODE_RX: {
            if(RadioState != MODE_RX){
                port_Pin_CE(0);
                rx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_RX;
            }
            break;
        }
        case MODE_TX: {
            if(RadioState != MODE_TX){
                port_Pin_CE(0);
                tx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_TX;
            }
            break;
        }		
    }
}


bool NRF24L01p::readable(){
    return (!get_fifo_flag_rx_empty()) ; 
}
bool NRF24L01p::writable(){
    return !get_fifo_flag_tx_full();
    
}
bool NRF24L01p::readableOnPipe(pipe_t pipe){
    
}


NRF24L01p::ErrorStatus_t NRF24L01p::writePayload(Payload_t *payload){
    ErrorStatus_t error;
    set_TX_pipe_address(payload->TxAddress);
    if(payload->UseAck == 1){
        write_tx_payload(payload->Data,payload->length);
        error = SUCCESS;
    }else{
        
        if(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled == 1){
            write_tx_payload_noack(payload->Data,payload->length); 
            error = SUCCESS;
        }
        else{
            error = ERROR; //feature dynamic payload with no ack is not enabled
        }
    }
    return error;
}

NRF24L01p::ErrorStatus_t NRF24L01p::writeAckPayload(Payload_t *payload){
    write_ack_payload(payload->pipe, payload->Data, payload->length);
}
NRF24L01p::ErrorStatus_t NRF24L01p::readPayload(Payload_t *payload){
    ErrorStatus_t error;                                  
    payload->TxAckPipe = get_rx_payload_pipe();
    
    if(payload->TxAckPipe>=0 && payload->TxAckPipe<=5){
        if(RadioConfig.FeatureDynamicPayloadEnabled == 1){
            payload->length = read_rx_payload_width();
        }else{
            payload->length = get_RX_pipe_width(payload->TxAckPipe);
        }
        read_rx_payload(payload->Data,payload->length);
        error = SUCCESS;
    }
    return error;
}


NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayload(Payload_t *payload){
    ErrorStatus_t error;
	RadioState_t originalState = RadioState;
	RadioMode(MODE_STANDBY);

	if(TxPipeAddress != payload->TxAddress){
		set_TX_pipe_address(payload->TxAddress);
		TxPipeAddress = payload->TxAddress;
	}

	if(payload->UseAck){
		if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
				enable_auto_ack(PIPE_P0, 1);
		}
		if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
				enable_rx_on_pipe(PIPE_P0, 1);
		}
		set_RX_pipe_address(PIPE_P0, payload->TxAddress);

		writePayload(payload);
		clear_data_sent_flag();
		while(1){
				RadioMode(MODE_TX);
				RadioMode(MODE_STANDBY);

				uint8_t status = get_status();

				if( (status & (_NRF24L01P_STATUS_TX_DS))  &&   (status & (_NRF24L01P_STATUS_RX_DR))   ){
					//printf("ACK with PAYLOAD\r\n");
                                                clear_data_sent_flag();
                                                clear_data_ready_flag();
						error = SUCCESS;
						readPayload(payload);
						payload->GotAck = 1;

						break;
				}

				else if( (status & (_NRF24L01P_STATUS_TX_DS))  &&   !(status & (_NRF24L01P_STATUS_RX_DR))   ){
					//printf("ACK ONLY\r\n");
					clear_data_sent_flag();
                                        error = SUCCESS;
					payload->GotAck = 1;
                                        payload->length = 0;
					break;
				}

				else if((status & (_NRF24L01P_STATUS_MAX_RT))){
                                        clear_max_retry_flag();
					error = ERROR;
                                        break;
				}
		}
		set_RX_pipe_address(PIPE_P0, RxPipeConfig[PIPE_P0].address);
		enable_auto_ack(PIPE_P0, RxPipeConfig[PIPE_P0].autoAckEnabled);
		enable_rx_on_pipe(PIPE_P0, RxPipeConfig[PIPE_P0].PipeEnabled);
	}
	else{
		error = writePayload(payload);
		if(error == ERROR) return error;
                clear_data_sent_flag();
                clear_max_retry_flag();
		while(1){
			RadioMode(MODE_TX);
			RadioMode(MODE_STANDBY);
           
			if(get_data_sent_flag()){
                            clear_data_sent_flag();
                            error = SUCCESS;
                            break;
			}
		}
	}
	//flush_tx();
	RadioMode(originalState);
	return error;

}
NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayload(Payload_t *payload){
	ErrorStatus_t error;
	error = readPayload(payload);
	if(error == ERROR){
		flush_rx();
	}

    return error;
}

#if (_NRF24L01P_INTERRUPT_FEATURE_API == 1)   

NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayloadInterruptHandled(Payload_t *payload){
	ErrorStatus_t error;
	RadioState_t originalState = RadioState;
	RadioMode(MODE_STANDBY);

	if(TxPipeAddress != payload->TxAddress){
		set_TX_pipe_address(payload->TxAddress);
		TxPipeAddress = payload->TxAddress;
	}

	if(payload->UseAck){
		if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
				enable_auto_ack(PIPE_P0, 1);
		}
		if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
				enable_rx_on_pipe(PIPE_P0, 1);
		}
		set_RX_pipe_address(PIPE_P0, payload->TxAddress);

		writePayload(payload);
		drFlag = 0;
		dsFlag = 0;
		mrFlag = 0;
		while(1){
				RadioMode(MODE_TX);
				RadioMode(MODE_STANDBY);

				if( (dsFlag)  &&   (drFlag)   ){
					//printf("ACK with PAYLOAD\r\n");
                                        clear_data_sent_flag();
                                        clear_data_ready_flag();
                                        error = SUCCESS;
                                        readPayload(payload);
                                        payload->GotAck = 1;
                                        break;
				}

				else if( (dsFlag)  &&   !(drFlag)   ){
					//printf("ACK ONLY\r\n");
					clear_data_sent_flag();
                                        error = SUCCESS;
					payload->GotAck = 1;
                                        payload->length = 0;
					break;
				}

				else if(mrFlag){
                                        clear_max_retry_flag();
					error = ERROR;
                                        break;
				}
		}
		set_RX_pipe_address(PIPE_P0, RxPipeConfig[PIPE_P0].address);
		enable_auto_ack(PIPE_P0, RxPipeConfig[PIPE_P0].autoAckEnabled);
		enable_rx_on_pipe(PIPE_P0, RxPipeConfig[PIPE_P0].PipeEnabled);
	}
	else{
		error = writePayload(payload);
		if(error == ERROR) return error;
                
		drFlag = 0;
		dsFlag = 0;
		mrFlag = 0;
		while(1){
			RadioMode(MODE_TX);
			RadioMode(MODE_STANDBY);
			if(get_data_sent_flag()){
                            clear_data_sent_flag();
                            error = SUCCESS;
			break;
			}
		}
	}
	//flush_tx();
	RadioMode(originalState);
	return error;

}

NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayloadInterruptHandled(Payload_t *payload){
	ErrorStatus_t error;
	error = readPayload(payload);
	if(error == ERROR){
		flush_rx();
	}

    return error;



}
void NRF24L01p::InterruptHandler(void){
	uint8_t status = get_status();

	if(status & _NRF24L01P_STATUS_RX_DR) drFlag = 1;
	else drFlag = 0;

	if(status & _NRF24L01P_STATUS_TX_DS) dsFlag = 1;
	else dsFlag = 0;

	if(status & _NRF24L01P_STATUS_MAX_RT) mrFlag = 1;
	else mrFlag = 0;

	clear_data_ready_flag();
}

#endif





#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)   

NRF24L01p::ErrorStatus_t NRF24L01p::fifo_init(fifo_t *f, Payload_t  *pld, unsigned int size){
    f->head = 0;
    f->tail = 0;
    f->size = size;
    f->payload = pld;
}

NRF24L01p::ErrorStatus_t NRF24L01p::fifo_read(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        f->tail++;
        if(f->tail == f->size){
            f->tail = 0;
        }
        retval =  SUCCESS;
    }
    else {
        retval =  ERROR;
    }

    return retval;
}
NRF24L01p::ErrorStatus_t NRF24L01p::fifo_peek(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        retval =  SUCCESS;
    }
    else {
        retval =  ERROR;
    }
    return retval;
}
NRF24L01p::ErrorStatus_t NRF24L01p::fifo_write(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if((f->head + 1 == f->tail) || (f->head+1  == f->size )&&(f->tail == 0)    ){
        retval =  ERROR;
    }
    else{
        f->payload[f->head] = *(pld);
        f->head++;
        if(f->head == f->size){
            f->head = 0;
        }
        retval =  SUCCESS;
    }
    return retval;
}


unsigned int NRF24L01p::fifo_waiting(fifo_t *f){
	if(f->head >= f->tail){
		return f->head - f->tail;
	}
	else{
		return (f->size - f->tail) + f->head;
	}
}
unsigned int NRF24L01p::fifo_freeSpace(fifo_t *f){
	return f->size - fifo_waiting(f);
}

NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayloadViaFifo(Payload_t *payload){
    return fifo_write(&TxFifo, payload);
}
NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayloadViaFifo(Payload_t *payload){
    return fifo_read(&RxFifo, payload);
}


NRF24L01p::ErrorStatus_t NRF24L01p::fifo_reset(fifo_t *f){
	f->head = 0;
	f->tail = 0;
}


void NRF24L01p::process(void){


    if( get_data_ready_flag() ){
        Payload_t RxPayload;
        ReceivePayload(&RxPayload);
        fifo_write(&RxFifo, &RxPayload );
        clear_data_ready_flag();
        while(readable() && fifo_freeSpace(&RxFifo) > 0){
            ReceivePayload(&RxPayload);
            fifo_write(&RxFifo, &RxPayload );
        }
    }
    

    
    if(fifo_waiting(&TxFifo) > 0){

        while(writable() && fifo_waiting(&TxFifo) > 0){
            Payload_t TxPayload;
            fifo_read(&TxFifo, &TxPayload);
            printf("now writing to send[%s][%d] to %#llx\r\n",TxPayload.Data, TxPayload.length, TxPayload.TxAddress );
            if( TransmitPayload(&TxPayload ) == ERROR){
                flush_tx();
            }
             flush_tx();//test
        }
    }

    hardwareCheck();

}

void NRF24L01p::processInterruptHandled(void){
    
    if( drFlag){
        Payload_t RxPayload;
        ReceivePayloadInterruptHandled(&RxPayload);
        fifo_write(&RxFifo, &RxPayload );
    	drFlag = 0;
        clear_data_ready_flag();
        while(readable() && fifo_freeSpace(&RxFifo) > 0){
            ReceivePayloadInterruptHandled(&RxPayload);
            fifo_write(&RxFifo, &RxPayload );
        }
    }
    
    if(fifo_waiting(&TxFifo) > 0){
        while(writable() && fifo_waiting(&TxFifo) > 0){
            Payload_t TxPayload;
            fifo_read(&TxFifo, &TxPayload);
            if(TransmitPayloadInterruptHandled(&TxPayload ) == ERROR){
                flush_tx();
            }
        }
    }

    hardwareCheck();
}


#endif

void NRF24L01p::hardwareCheck(){
    if(get_status() == 0) {
        printf("Radio Disconnected\r\n");
        while(get_status() == 0){
            ReInitialize();
            port_DelayMs(1000);
        }
        printf("Radio Reconnected\r\n");
    }

    //ReInitialize();
    //ResetConfigValues(&RadioConfig, RxPipeConfig);
}


float NRF24L01p::TestCarrierQuality(unsigned int frequencyOffset, unsigned int readtimes){
    freqOffset(frequencyOffset);
    int TotalRpdCount = 0;
    int i = 0;
        for(i=0;i<readtimes;i++){
        if(rpd() == 1){
            TotalRpdCount++;
            //printf(".");
        }
    }
    float careerFreePercent = float((  (float)TotalRpdCount/(float)readtimes)*100.0);
    //printf("Freq : %d ;RPD high : %d/%d\t:\t[%6.3f] \r\n", 2400+frequencyOffset, TotalRpdCount,readtimes , 100.0 - careerFreePercent );
    
    //printf("setting back to original frequency: %d\r\n", RadioConfig.frequencyOffset + 2400);
    freqOffset(RadioConfig.frequencyOffset);
    
    return careerFreePercent;
}
void NRF24L01p::GenerateCarrierQualityReport(unsigned int readtimes){
    int j=0;
    for(j=0;j<125;j++){
        //int engageTimes = CarrierQualityTest(i,readtimes);
        
        freqOffset(j);
        int TotalRpdCount = 0;
        int i = 0;
            for(i=0;i<readtimes;i++){
            if(rpd() == 1){
                TotalRpdCount++;
                //printf(".");
            }
        }
        float quality = float((  (float)TotalRpdCount/(float)readtimes)*100.0);
        printf("Freq : %d ;RPD high : %d/%d\t:\t[%6.3f] \r\n", 2400+j, TotalRpdCount,readtimes , 100.0 - quality );
        
    }
    printf("setting back to original frequency: %d\r\n", RadioConfig.frequencyOffset + 2400);
    freqOffset(RadioConfig.frequencyOffset);
}






/*
 
 NRF24L01p::NRF24L01p() {
    
    RadioState = MODE_UNKNOWN;
    
    
    RadioConfig.DataReadyInterruptEnabled = 1;
    RadioConfig.DataSentInterruptEnabled = 1;
    RadioConfig.MaxRetryInterruptEnabled = 1;
    RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPowerDb = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;
    
    RxPipeConfig[0].address = 0xe7e7e7e7e7;
    RxPipeConfig[1].address = 0xc2c2c2c2c2;
    RxPipeConfig[2].address = 0xc2c2c2c2c3;
    RxPipeConfig[3].address = 0xc2c2c2c2c4;
    RxPipeConfig[4].address = 0xc2c2c2c2c5;
    RxPipeConfig[5].address = 0xc2c2c2c2c6;
    
}

NRF24L01p::NRF24L01p(const NRF24L01p& orig) {
}

NRF24L01p::~NRF24L01p() {
}

void NRF24L01p::ReInitialize(){
    port_Pin_CE(0);
    port_Pin_CSN(0);

    port_DelayMs(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clearDataReadyFlag();
    clearDataSentFlag();
    clearMaxRetryFlag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);
    
    dataReadyInterruptMask(RadioConfig.DataReadyInterruptEnabled);
    dataSentInterruptMask(RadioConfig.DataSentInterruptEnabled);
    maxRetryInterruptMask(RadioConfig.MaxRetryInterruptEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled);
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled);
    maxAutoRetransmissionCount(RadioConfig.AutoReTransmissionCount);
    autoRetransmissionDelay(RadioConfig.AutoReTransmitDelayX250us);        
    DataRate(RadioConfig.datarate);
    
    int i;
    for(i=0;i<6;i++){
        RxOnPipe((pipe_t)i,RxPipeConfig[i].PipeEnabled );
        autoAckOnPipe((pipe_t)i,RxPipeConfig[i].autoAckEnabled );
        dynamicPayloadOnPipe((pipe_t)i,RxPipeConfig[i].dynamicPayloadEnabled);
        rxPipeAddress((pipe_t)i,RxPipeConfig[i].address);
    }
#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)      
    fifo_init(&TxFifo, TxFifoBuffer, 10);
    fifo_init(&RxFifo, RxFifoBuffer, 10);
#endif
}
void NRF24L01p::Initialize(){

    port_Initialize();

    port_Pin_CE(0);
    port_Pin_CSN(0);

    port_DelayMs(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clear_data_ready_flag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);



    dataReadyInterruptMask(RadioConfig.DataReadyInterruptEnabled);
    dataSentInterruptMask(RadioConfig.DataSentInterruptEnabled);
    maxRetryInterruptMask(RadioConfig.MaxRetryInterruptEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled);
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled);
    maxAutoRetransmissionCount(RadioConfig.AutoReTransmissionCount);
    autoRetransmissionDelay(RadioConfig.AutoReTransmitDelayX250us);        
    DataRate(RadioConfig.datarate);
    freqOffset(RadioConfig.frequencyOffset);
    

    RfPower(RadioConfig.RfPowerDb);
    pllLock(RadioConfig.PllLock);
    contWave(RadioConfig.ContWaveEnabled);
    dynamicPayloadFeature(RadioConfig.FeatureDynamicPayloadEnabled);
    payloadWithAckFeature(RadioConfig.FeaturePayloadWithAckEnabled );
    dynamicPayloadWithNoAck(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled );
    
    
    int i;
    for(i=0;i<6;i++){
        RxOnPipe((pipe_t)i,RxPipeConfig[i].PipeEnabled );
        autoAckOnPipe((pipe_t)i,RxPipeConfig[i].autoAckEnabled );
        dynamicPayloadOnPipe((pipe_t)i,RxPipeConfig[i].dynamicPayloadEnabled);
        rxPipeAddress((pipe_t)i,RxPipeConfig[i].address);
    }
    
#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)  
    fifo_init(&TxFifo, TxFifoBuffer, 10);
    fifo_init(&RxFifo, RxFifoBuffer, 10);
#endif    
}

NRF24L01p::RadioState_t NRF24L01p::RadioMode(){
    bool _ce = port_Pin_CE();
    uint8_t _config = read_register(_NRF24L01P_REG_CONFIG)  ;
    bool _pwr = (_config>>1)&0x01;
    bool _txrx = (_config>>0)&0x01;


    if(_pwr == 0){
        return MODE_POWER_DOWN;
    }
    else{
        if(_ce == 0){
            return MODE_STANDBY;
        }
        else{
            if(_txrx==0){
                return MODE_TX;
            }else{
                return MODE_RX;
            }
        }
    } 
}

void NRF24L01p::RadioMode(NRF24L01p::RadioState_t mode){
    RadioState = RadioMode();
    
    switch(mode){
        case MODE_POWER_DOWN: {
            power_down();
            port_Pin_CE(0);
            RadioState = MODE_POWER_DOWN;
            break;
        }
        case MODE_STANDBY: {
            if(RadioState == MODE_POWER_DOWN){
                    power_up();
                    port_DelayUs(_NRF24L01P_TIMING_Tpd2stby_us);
            }
            else{
                    port_Pin_CE(0);
            }
            RadioState = MODE_STANDBY;
            break;
        }
        case MODE_RX: {
            if(RadioState != MODE_RX){
                //if(RadioState == MODE_TX){
                //    RadioMode(MODE_STANDBY);
                //}
                
                port_Pin_CE(0);
                rx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_RX;
            }
            break;
        }
        case MODE_TX: {
            if(RadioState != MODE_TX){
                //if(RadioState == MODE_RX){
                //    RadioMode(MODE_STANDBY);
                //}
                port_Pin_CE(0);
                tx_mode();
                port_Pin_CE(1);
                port_DelayUs(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_TX;
            }
            break;
        }		
    }
}


bool NRF24L01p::readable(){
    return (!get_fifo_flag_rx_empty()) ; 
}
bool NRF24L01p::writable(){
    return !get_fifo_flag_tx_full();
    
}
bool NRF24L01p::readableOnPipe(pipe_t pipe){
    
}


NRF24L01p::ErrorStatus_t NRF24L01p::writePayload(Payload_t *payload){
    ErrorStatus_t error;
    set_TX_pipe_address(payload->TxAddress); /////////////////////////////////////////////////////////////CHECK IF NEEDED TO REMOVE
    
    if(payload->UseAck == 1){
        write_tx_payload(payload->Data,payload->TxDataLen);
        error = SUCCESS;
        
    }else{
        
        if(RadioConfig.FeatureDynamicPayloadWithNoAckEnabled == 1){
            write_tx_payload_noack(payload->Data,payload->TxDataLen); 
            error = SUCCESS;
        }
        else{
            error = ERROR; //feature dynamic payload with no ack is not enabled
        }
    }
    return error;
}

NRF24L01p::ErrorStatus_t NRF24L01p::writeAckPayload(Payload_t *payload){
    write_ack_payload(payload->TxAckPipe, payload->Data, payload->TxDataLen);
}
NRF24L01p::ErrorStatus_t NRF24L01p::readPayload(Payload_t *payload){
    ErrorStatus_t error;                                  
    payload->RxPipe = get_rx_payload_pipe();
    
    if(payload->RxPipe>=0 && payload->RxPipe<=5){
        if(RadioConfig.FeatureDynamicPayloadEnabled == 1){
            payload->RxDataLen = read_rx_payload_width();
        }else{
            payload->RxDataLen = get_RX_pipe_width(payload->RxPipe);
        }
        read_rx_payload(payload->Data,payload->RxDataLen);
        error = SUCCESS;
    }
    return error;
}


NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayload(Payload_t *payload){
    ErrorStatus_t error;
    RadioState_t originalState = RadioState;
    RadioMode(MODE_STANDBY);

    if(TxPipeAddress != payload->TxAddress){
            set_TX_pipe_address(payload->TxAddress);
            TxPipeAddress = payload->TxAddress;
    }
    
    if(payload->UseAck){
            if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
                            enable_auto_ack(PIPE_P0, 1);
            }
            if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
                            enable_rx_on_pipe(PIPE_P0, 1);
            }
            set_RX_pipe_address(PIPE_P0, payload->TxAddress);
            writePayload(payload);
            

            clear_data_sent_flag();
            
            while(1){
                    RadioMode(MODE_TX);
                    RadioMode(MODE_STANDBY);
                    
                    uint8_t status = get_status();

                    if( (status & (_NRF24L01P_STATUS_TX_DS))  &&   (status & (_NRF24L01P_STATUS_RX_DR))   ){
                            //printf("ACK with PAYLOAD\r\n");
                            clear_data_sent_flag();
                            clear_data_ready_flag();
                            error = SUCCESS;
                            readPayload(payload);
                            payload->GotAck = 1;
                            break;
                    }

                    else if( (status & (_NRF24L01P_STATUS_TX_DS))  &&   !(status & (_NRF24L01P_STATUS_RX_DR))   ){
                            //printf("ACK ONLY\r\n");
                            clear_data_sent_flag();
                            error = SUCCESS;
                            payload->GotAck = 1;
                            payload->RxDataLen = 0;
                            break;
                    }

                    else if((status & (_NRF24L01P_STATUS_MAX_RT))){
                            //printf("HARD MAX RETRY\r\n");
                            clear_max_retry_flag();
                            //reuse_tx_payload();
                            error = ERROR;
                            break;

                            //if(get_plos_count()>=payload->retransmitCount){
                            //    printf("SOFT MAX RETRY\r\n");
                                //set_frequency_offset(RadioConfig.frequencyOffset);
                                //error = ERROR;
                                //break;
                            //}
                            //reuse_tx_payload();
                    }

                            
            }

            set_RX_pipe_address(PIPE_P0, RxPipeConfig[PIPE_P0].address);
            enable_auto_ack(PIPE_P0, RxPipeConfig[PIPE_P0].autoAckEnabled);
            enable_rx_on_pipe(PIPE_P0, RxPipeConfig[PIPE_P0].PipeEnabled);
    }
    else{
            error = writePayload(payload);
            if(error == ERROR) return error;
            clear_data_sent_flag();
            clear_max_retry_flag();
            while(1){
                
                    RadioMode(MODE_TX);
                    RadioMode(MODE_STANDBY);
                    //printf("status = %#2x\r\n", get_status());

                    if(get_data_sent_flag()){
                        clear_data_sent_flag();
                        error = SUCCESS;
                        break;
                    }
            }
    }

    RadioMode(originalState);
    return error;

}
NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayload(Payload_t *payload){
	ErrorStatus_t error;
	error = readPayload(payload);
	if(error == ERROR){
		flush_rx();
	}

    return error;
}

#if (_NRF24L01P_INTERRUPT_FEATURE_API == 1)   

NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayloadInterruptHandled(Payload_t *payload){
	ErrorStatus_t error;
	RadioState_t originalState = RadioState;
	RadioMode(MODE_STANDBY);

	if(TxPipeAddress != payload->TxAddress){
		set_TX_pipe_address(payload->TxAddress);
		TxPipeAddress = payload->TxAddress;
	}

	if(payload->UseAck){
		if(RxPipeConfig[PIPE_P0].autoAckEnabled == 0){
				enable_auto_ack(PIPE_P0, 1);
		}
		if(RxPipeConfig[PIPE_P0].PipeEnabled == 0){
				enable_rx_on_pipe(PIPE_P0, 1);
		}
		set_RX_pipe_address(PIPE_P0, payload->TxAddress);

		writePayload(payload);
		drFlag = 0;
		dsFlag = 0;
		mrFlag = 0;
		while(1){
				RadioMode(MODE_TX);
				RadioMode(MODE_STANDBY);

				if( (dsFlag)  &&   (drFlag)   ){
					//printf("ACK with PAYLOAD\r\n");
                                        clear_data_sent_flag();
                                        clear_data_ready_flag();
                                        error = SUCCESS;
                                        readPayload(payload);
                                        payload->GotAck = 1;
                                        break;
				}

				else if( (dsFlag)  &&   !(drFlag)   ){
					//printf("ACK ONLY\r\n");
					clear_data_sent_flag();
                                        error = SUCCESS;
					payload->GotAck = 1;
                                        payload->RxDataLen = 0;
					break;
				}

				else if(mrFlag){
					clear_max_retry_flag();
					error = ERROR;
                                        break;
                                        
                                        //if(get_plos_count()>=payload->SoftRtransmitCount){
                                        //        set_frequency_offset(RadioConfig.frequencyOffset);
                                        //        error = ERROR;
                                        //        break;
					//}
				}
		}
		set_RX_pipe_address(PIPE_P0, RxPipeConfig[PIPE_P0].address);
		enable_auto_ack(PIPE_P0, RxPipeConfig[PIPE_P0].autoAckEnabled);
		enable_rx_on_pipe(PIPE_P0, RxPipeConfig[PIPE_P0].PipeEnabled);
	}
	else{
		error = writePayload(payload);
		if(error == ERROR) return error;
                
		drFlag = 0;
		dsFlag = 0;
		mrFlag = 0;
		while(1){
			RadioMode(MODE_TX);
			RadioMode(MODE_STANDBY);
			if(get_data_sent_flag()){
                            clear_data_sent_flag();
                            error = SUCCESS;
			break;
			}
		}
	}
	//flush_tx();
	RadioMode(originalState);
	return error;

}

NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayloadInterruptHandled(Payload_t *payload){
	ErrorStatus_t error;
	error = readPayload(payload);
	if(error == ERROR){
		flush_rx();
	}

    return error;



}
void NRF24L01p::InterruptHandler(void){
	uint8_t status = get_status();

	if(status & _NRF24L01P_STATUS_RX_DR) drFlag = 1;
	else drFlag = 0;

	if(status & _NRF24L01P_STATUS_TX_DS) dsFlag = 1;
	else dsFlag = 0;

	if(status & _NRF24L01P_STATUS_MAX_RT) mrFlag = 1;
	else mrFlag = 0;

	clear_data_ready_flag();
}

#endif





#if (_NRF24L01P_USE_SOFTWARE_FIFO_API == 1)   

NRF24L01p::ErrorStatus_t NRF24L01p::fifo_init(fifo_t *f, Payload_t  *pld, unsigned int size){
    f->head = 0;
    f->tail = 0;
    f->size = size;
    f->payload = pld;
}

NRF24L01p::ErrorStatus_t NRF24L01p::fifo_read(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        f->tail++;
        if(f->tail == f->size){
            f->tail = 0;
        }
        retval =  SUCCESS;
    }
    else {
        retval =  ERROR;
    }

    return retval;
}
NRF24L01p::ErrorStatus_t NRF24L01p::fifo_peek(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        retval =  SUCCESS;
    }
    else {
        retval =  ERROR;
    }
    return retval;
}
NRF24L01p::ErrorStatus_t NRF24L01p::fifo_write(fifo_t *f, Payload_t  *pld){
	ErrorStatus_t retval;
    if((f->head + 1 == f->tail) || (f->head+1  == f->size )&&(f->tail == 0)    ){
        retval =  ERROR;
    }
    else{
        f->payload[f->head] = *(pld);
        f->head++;
        if(f->head == f->size){
            f->head = 0;
        }
        retval =  SUCCESS;
    }
    return retval;
}


unsigned int NRF24L01p::fifo_waiting(fifo_t *f){
	if(f->head >= f->tail){
		return f->head - f->tail;
	}
	else{
		return (f->size - f->tail) + f->head;
	}
}
unsigned int NRF24L01p::fifo_freeSpace(fifo_t *f){
	return f->size - fifo_waiting(f);
}

NRF24L01p::ErrorStatus_t NRF24L01p::TransmitPayloadViaFifo(Payload_t *payload){
    return fifo_write(&TxFifo, payload);
}
NRF24L01p::ErrorStatus_t NRF24L01p::ReceivePayloadViaFifo(Payload_t *payload){
    return fifo_read(&RxFifo, payload);
}


NRF24L01p::ErrorStatus_t NRF24L01p::fifo_reset(fifo_t *f){
	f->head = 0;
	f->tail = 0;
}


void NRF24L01p::process(void){


    if( get_data_ready_flag() ){
        Payload_t RxPayload;
        ReceivePayload(&RxPayload);
        fifo_write(&RxFifo, &RxPayload );
        clear_data_ready_flag();
        while(readable() && fifo_freeSpace(&RxFifo) > 0){
            ReceivePayload(&RxPayload);
            fifo_write(&RxFifo, &RxPayload );
        }
    }
    

    
    if(fifo_waiting(&TxFifo) > 0){

        while(writable() && fifo_waiting(&TxFifo) > 0){
            Payload_t TxPayload;
            fifo_read(&TxFifo, &TxPayload);
            printf("now writing to send[%s][%d] to %#llx\r\n",TxPayload.Data, TxPayload.TxDataLen, TxPayload.TxAddress );
            if( TransmitPayload(&TxPayload ) == ERROR){
                flush_tx();
            }
             flush_tx();//test
        }
    }

    hardwareCheck();

}

void NRF24L01p::processInterruptHandled(void){
    
    if( drFlag){
        Payload_t RxPayload;
        ReceivePayloadInterruptHandled(&RxPayload);
        fifo_write(&RxFifo, &RxPayload );
    	drFlag = 0;
        clear_data_ready_flag();
        while(readable() && fifo_freeSpace(&RxFifo) > 0){
            ReceivePayloadInterruptHandled(&RxPayload);
            fifo_write(&RxFifo, &RxPayload );
        }
    }
    
    if(fifo_waiting(&TxFifo) > 0){
        while(writable() && fifo_waiting(&TxFifo) > 0){
            Payload_t TxPayload;
            fifo_read(&TxFifo, &TxPayload);
            if(TransmitPayloadInterruptHandled(&TxPayload ) == ERROR){
                flush_tx();
            }
        }
    }

    hardwareCheck();
}


#endif

void NRF24L01p::hardwareCheck(){
    if(get_status() == 0) {
        printf("Radio Disconnected\r\n");
        while(get_status() == 0){
            ReInitialize();
            port_DelayMs(1000);
        }
        printf("Radio Reconnected\r\n");
    }

    //ReInitialize();
    //ResetConfigValues(&RadioConfig, RxPipeConfig);
}











float NRF24L01p::TestCarrierQuality(unsigned int frequencyOffset, unsigned int readtimes){
    freqOffset(frequencyOffset);
    int TotalRpdCount = 0;
    int i = 0;
        for(i=0;i<readtimes;i++){
        if(rpd() == 1){
            TotalRpdCount++;
            //printf(".");
        }
    }
    float careerFreePercent = float((  (float)TotalRpdCount/(float)readtimes)*100.0);
    //printf("Freq : %d ;RPD high : %d/%d\t:\t[%6.3f] \r\n", 2400+frequencyOffset, TotalRpdCount,readtimes , 100.0 - careerFreePercent );
    
    //printf("setting back to original frequency: %d\r\n", RadioConfig.frequencyOffset + 2400);
    freqOffset(RadioConfig.frequencyOffset);
    
    return careerFreePercent;
}
void NRF24L01p::GenerateCarrierQualityReport(unsigned int readtimes){
    int j=0;
    for(j=0;j<125;j++){
        //int engageTimes = CarrierQualityTest(i,readtimes);
        
        freqOffset(j);
        int TotalRpdCount = 0;
        int i = 0;
            for(i=0;i<readtimes;i++){
            if(rpd() == 1){
                TotalRpdCount++;
                //printf(".");
            }
        }
        float quality = float((  (float)TotalRpdCount/(float)readtimes)*100.0);
        printf("Freq : %d ;RPD high :\t[%6.3f]\t\r\n", 2400+j, 100.0 - quality );

    }
    printf("setting back to original frequency: %d\r\n", RadioConfig.frequencyOffset + 2400);
    freqOffset(RadioConfig.frequencyOffset);
}
 
 
 */
