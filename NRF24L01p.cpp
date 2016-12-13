/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01p.cpp
 * Author: emon1
 * 
 * Created on December 11, 2016, 10:45 PM
 */

#include "NRF24L01p.h"

#include <stdio.h>
#include <string.h>
#include <cstdlib>

NRF24L01p::NRF24L01p() {
}

NRF24L01p::NRF24L01p(const NRF24L01p& orig) {
}

NRF24L01p::~NRF24L01p() {
}

int NRF24L01p::fifo_init(fifo_t *f, Payload_t  *pld, unsigned int size){
    f->head = 0;
    f->tail = 0;
    f->size = size;
    f->payload = pld;
}

int NRF24L01p::fifo_read(fifo_t *f, Payload_t  *pld){
    int retval = 0;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        f->tail++;
        if(f->tail == f->size){
            f->tail = 0;
        }
        retval =  0;
    }
    else {
        retval =  -1;
    }
    
    return retval;
}
int NRF24L01p::fifo_peekNext(fifo_t *f, Payload_t  *pld){
    int retval;
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        retval =  0;
    }
    else {
        retval =  -1;
    }
    return retval;
}
int NRF24L01p::fifo_write(fifo_t *f, Payload_t  *pld){
    int retval;
    if((f->head + 1 == f->tail) || (f->head+1  == f->size )&&(f->tail == 0)    ){
        retval =  -1;
    }
    else{
        f->payload[f->head] = *(pld);
        f->head++;
        if(f->head == f->size){
            f->head = 0;
        }
        retval =  0;
    }
    return retval;
}






void NRF24L01p::initialize(){
    
    Port_Initialize();

    startup();
    default_config();
    

    fifo_init(&TxFifo, TxFifoBuffer, NRF24L01P_FIFO_SIZE);
    fifo_init(&RxFifo, RxFifoBuffer, NRF24L01P_FIFO_SIZE);
    
    TxAddr = 0xe7e7e7e7e7;
}



int NRF24L01p::startup(){
    Port_CE_pin(0);
    Port_CSN_pin(0);

    Port_Delay_ms(_NRF24L01P_TIMING_PowerOnReset_ms);

    RadioMode(MODE_POWER_DOWN);
    RadioMode(MODE_RX);

    clear_data_ready_flag();
    flush_rx();
    flush_tx();

    uint8_t status_rst_val = 0x70;//reset status
    write_register(_NRF24L01P_REG_STATUS, &status_rst_val,1);
    uint8_t config_rst_val = 0x0b;//reset config
    write_register(_NRF24L01P_REG_CONFIG, &config_rst_val,1);

    disable_auto_ack_all_pipes();
    disable_dynamic_payload_all_pipe();/////////ALSO CREEATE FOR DISABLE AUTO ACK FOR ALL PIPE
    return 0;
}

int NRF24L01p::default_config(){

    enable_dynamic_payload();
    enable_payload_with_ack();

    enable_auto_ack(PIPE_P0);
    enable_auto_ack(PIPE_P1);
    enable_auto_ack(PIPE_P2);
    enable_auto_ack(PIPE_P3);
    enable_auto_ack(PIPE_P4);
    enable_auto_ack(PIPE_P5);

    enable_dynamic_payload_pipe(PIPE_P0);
    enable_dynamic_payload_pipe(PIPE_P1);
    enable_dynamic_payload_pipe(PIPE_P2);
    enable_dynamic_payload_pipe(PIPE_P3);
    enable_dynamic_payload_pipe(PIPE_P4);
    enable_dynamic_payload_pipe(PIPE_P5);

    enable_rx_on_pipe(PIPE_P0);
    enable_rx_on_pipe(PIPE_P1);
    enable_rx_on_pipe(PIPE_P2);
    enable_rx_on_pipe(PIPE_P3);
    enable_rx_on_pipe(PIPE_P4);
    enable_rx_on_pipe(PIPE_P5);

    set_auto_retransmission_count(15);
    set_auto_retransmission_delay(15);

    set_DataRate(RF_SETUP_RF_DR_250KBPS);
    set_DataRate(RF_SETUP_RF_DR_2MBPS);
    return 0;
}

void NRF24L01p::RadioMode(NRF24L01p::StateType mode){
    switch(mode){
        case MODE_POWER_DOWN: {
            power_down();
            Port_CE_pin(0);
            RadioState = MODE_POWER_DOWN;
            break;
        }
        case MODE_STANDBY: {
            if(RadioState == MODE_POWER_DOWN){
                    power_up();
                    Port_Delay_us(_NRF24L01P_TIMING_Tpd2stby_us);
            }
            else{
                    Port_CE_pin(0);
            }
            RadioState = MODE_STANDBY;
            break;
        }
        case MODE_RX: {
            if(RadioState != MODE_RX){
                Port_CE_pin(0);
                rx_mode();
                Port_CE_pin(1);
                Port_Delay_us(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_RX;
            }

            break;
        }
        case MODE_TX: {
            if(RadioState != MODE_TX){
                Port_CE_pin(0);
                tx_mode();
                Port_CE_pin(1);
                Port_Delay_us(_NRF24L01P_TIMING_Tstby2a_us);
                RadioState = MODE_TX;
            }


            break;
        }		
    }
}


bool NRF24L01p::readable(){
    return get_data_ready_flag() || !get_fifo_flag_rx_empty();
}
bool NRF24L01p::writable(){
    return !get_fifo_flag_tx_empty();
}





int NRF24L01p::write_payload_to_send(uint8_t *data, int datalen){
    if ( datalen <= 0 ) return 0;
    if ( datalen > _NRF24L01P_FIFO_SIZE ) datalen = _NRF24L01P_FIFO_SIZE;

    if(get_tx_fifo_full_flag()) return -1;
    //while(_nrf24l01p_get_tx_fifo_full_flag());
    write_tx_payload(data,datalen);
    return 0;

}

int NRF24L01p::write_payload_to_send_to_address(uint64_t address, uint8_t *data, int datalen){
	//_nrf24l01p_disable_payload_with_ack();
	if(address != TxAddr){
            set_TX_pipe_address(address);
            TxAddr = address;
            //set_RX_pipe_address(PIPE_P0, address);
        }
	return write_payload_to_send(data,datalen);
}

int NRF24L01p::write_payload_to_send_to_address_ack(uint64_t address, uint8_t *data, int datalen){
	//enable_payload_with_ack();
        if(address != TxAddr){
            set_TX_pipe_address(address);
            TxAddr = address;
            set_RX_pipe_address(PIPE_P0, address);
        }
	return write_payload_to_send(data,datalen);
}


int NRF24L01p::write_payload_to_send_to_address_noack(pipeAddrType_t address, uint8_t *data, int datalen){
    	enable_payload_with_ack();
        
        if(address != TxAddr){
            set_TX_pipe_address(address);
            TxAddr = address;
        }
	//_nrf24l01p_set_RX_pipe_address(_NRF24L01P_PIPE_P0, address);
        write_tx_payload_noack(data,datalen);  
        return 0;

}





bool NRF24L01p::readableOnPipe(pipe_t pipe){
	bool flag = 0;
	if((pipe >=0)   && (pipe <=5)){
		int status = get_status();
		if(   (status&_NRF24L01P_STATUS_RX_DR)  && ((status&_NRF24L01P_STATUS_RX_P_NO)>>1)==pipe){
			flag = 1;
		}
		else{
			flag = 0;
		}
	}
	return flag;
}





int NRF24L01p::read_payload(pipe_t pipe, uint8_t *data, int datalen){
	int rxPayloadWidth;
	if ( ( pipe < 0 ) || ( pipe > 5 ) ) {
            return -1;
	}
	if (readableOnPipe(pipe) ) {
            rxPayloadWidth = _NRF24L01P_FIFO_SIZE;
            if ( ( rxPayloadWidth < 0 ) || ( rxPayloadWidth > _NRF24L01P_FIFO_SIZE ) ) {
                flush_rx();
            }
            else{
                read_rx_payload(data,rxPayloadWidth);
                if(get_fifo_flag_rx_empty()) {
                        clear_data_ready_flag();
                }
            }
            return rxPayloadWidth;
	}
	//if RX FIFO is full even after reading data, then flush RX FIFO
	if(get_fifo_flag_rx_full()){
            clear_data_ready_flag();
            flush_rx();
	}
	return 0;	
}



int NRF24L01p::read_payload_dyn(pipe_t pipe, uint8_t *data){
    int rxPayloadWidth;
    if ( ( pipe < 0 ) || ( pipe > 5 ) ) {
            return -1;
    }
    if (readableOnPipe(pipe) ) {
        rxPayloadWidth = read_rx_payload_width();
        if ( ( rxPayloadWidth < 0 ) || ( rxPayloadWidth > _NRF24L01P_FIFO_SIZE ) ) {
            flush_rx();
        }
        else{
            read_rx_payload(data,rxPayloadWidth);
            if(get_fifo_flag_rx_empty()) {
                clear_data_ready_flag();
            }
        }
        return rxPayloadWidth;
    }
    else {//if pipe not readable
            return -1;
    }
    return 0;
}


void NRF24L01p::PRX(){
    if(readable()){
        StateType  originalState = RadioState;
        while(1){
            uint8_t rxData[32];
            pipe_t pipe =  get_rx_payload_pipe();
            int width = read_payload_dyn(pipe, rxData);
            rxData[width] = '\0';
            if(width>0){
                Payload_t payload;
                strcpy((char*)payload.data, (char*)rxData);
                //memcpy(payload.data,rxData,32);
                payload.RxPipe = (pipe_t) pipe;
                if(fifo_write(&RxFifo, &payload) <= 0)break;
            }
            if(get_fifo_flag_rx_empty()) break;
        }
        RadioMode(originalState);
    }
}


void NRF24L01p::PTX(){
    Payload_t payload;
    int r = fifo_read(&TxFifo, &payload);
    if(r == -1){
        return;
    }
    else{
        //enable_payload_with_ack();
        write_payload_to_send_to_address_ack(payload.TxAddr, payload.data, strlen(payload.data));

        StateType  originalState = RadioState;

        if(writable() && !get_fifo_flag_rx_full())   {
            clear_data_sent_flag();
            while(1){
                RadioMode(MODE_TX);   
                RadioMode(MODE_STANDBY);   
                
                if(get_data_sent_flag()|| !writable()  )break;
                
                if(get_max_retry_flag()){
                    clear_max_retry_flag();
                    
                    if(get_plos_count()>=15){
                        set_frequency_offset(2);
                        break;
                    }
                }
            }
            
        //RadioMode(MODE_STANDBY);
        //clear data sent flag
        clear_data_sent_flag();

        //if got ack packet, just flush it
        if(get_data_ready_flag()){
            //do what needs to be done with the ACK payload here
            flush_rx();//if you want to flush RX, (use only if the PTX started when RX was empty)
        }

        //restore original machine state
        RadioMode(originalState);
        flush_tx();
           
        } 
    }
}

