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



NRF24L01p::NRF24L01p() {
}

NRF24L01p::NRF24L01p(const NRF24L01p& orig) {
}

NRF24L01p::~NRF24L01p() {
}

int NRF24L01p::fifo_init(fifo_t *f, Payload_t  *pld){
    f->head = 0;
    f->tail = 0;
    f->size = NRF24L01P_FIFO_SIZE;
    f->payload = pld;
}

int NRF24L01p::fifo_read(fifo_t *f, Payload_t  *pld){
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        f->tail++;
        if(f->tail == f->size){
            f->tail = 0;
        }
        return 0;
    }
    else {
        return -1;
    }
}
int NRF24L01p::fifo_peekNext(fifo_t *f, Payload_t  *pld){
    if(f->tail != f->head){
        *(pld) = (f->payload[f->tail]);
        return 0;
    }
    else {
        return -1;
    }

}
int NRF24L01p::fifo_write(fifo_t *f, Payload_t  *pld){
    if((f->head + 1 == f->tail) || (f->head  == f->size - 1)&&(f->tail == 0)    ){
        return -1;
    }
    else{
        (f->payload[f->head]) = (*pld);
        f->head++;
        if(f->head == f->size){
            f->head = 0;
        }
        return 0;
    }

}






void NRF24L01p::initialize(){
    
    Port_Initialize();

    startup();
    default_config();
    

    
    fifo_init(&TxFifo, TxFifoBuffer);
    fifo_init(&RxFifo, RxFifoBuffer);
    
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
    return get_data_ready_flag();
}
bool NRF24L01p::writable(){
    return !get_fifo_flag_tx_empty();
}

void NRF24L01p::TxFifo2RadioTxFifo(){
    Payload_t pld;
    Payload_t pldN;
    uint64_t TxAddr;
    if((fifo_peekNext(&TxFifo, &pld) == -1)){
        printf("am quitting this shit\r\n");
        return;
    }else{
        TxAddr = pld.TxAddr;
        printf("MAGIC MAGIC MAGIC %llx\r\n", TxAddr);
    }
    
    
    while(1){
        if(get_tx_fifo_full_flag()) {
            printf("-----tx fifo full\r\n");
            break;// if TX fifo is full in Radio, then break
        }
        
        if(fifo_read(&TxFifo, &pld) == -1) {
            printf("-----no data to write\r\n");
            break;//read from FIFO to pld. if -1, means no data in fifo. then break
        }
        else {
            printf("-----has data now writing\r\n");
            write_tx_payload(pld.data,sizeof(pld.data));  //write the payload read from fifo into Radio Tx fifo
            //set_TX_pipe_address(pld.TxAddr);
            //set_RX_pipe_address(PIPE_P0, pld.TxAddr);
        }
        
        if((fifo_peekNext(&TxFifo, &pldN) == -1)){
            printf("-----peeked but no data\r\n");
            break; //peek into the next FIFO payload. if -1, means no new data
        }
        else{
            
            if(pldN.TxAddr != pld.TxAddr  ){
                printf("-----peeked.next payload address not matching\r\n");
                break;//if new data, then check if the address is same as the one sent. if not , break
            }else{
                printf("-----peeked.next payload address MATCHED\r\n");
            }
        }
    }

    //enable_payload_with_ack();
    set_TX_pipe_address(TxAddr);
    set_RX_pipe_address(PIPE_P0, TxAddr);
    //printf("*****CONFIRMING Tx address is %llx\r\n", get_TX_pipe_address());
    
}


void NRF24L01p::RadioRxFifo2RxFifo(){

}




int NRF24L01p::RX(){

}
int NRF24L01p::TX(){
    
}


int NRF24L01p::sendSingleWithAck(Payload_t *payload){
    int retval;

    flush_tx();
    flush_rx();


    enable_payload_with_ack();
    set_TX_pipe_address(payload->TxAddr);
    set_RX_pipe_address(PIPE_P0, payload->TxAddr);
    write_tx_payload(payload->data,sizeof(payload->data));
    
    
    //backup original machine state
    StateType originalMode = RadioState;

    //has data to write and no data to read. Do not enter PTX with data in PRX payload.
    //This is because, if the PRX is full, then sending a data and will forever wait for ACK
    //and it will never get the ACK because the PRX fifo is full
    if(writable() && !readable()){
        //switching to STANDBY to avoid data reception during state check (ATOMIC state check)
        RadioMode(MODE_STANDBY);

        //clear data sent flag before the next packet is sent
        clear_data_sent_flag();
        //strobe CE for single transmission
        RadioMode(MODE_TX);


        //uint8_t observe;

        while(1){
           //break when DS flag is set and a single payload is sent or all data on RX FIFO sent
            if(get_data_sent_flag() ){
                retval = 0;
                printf("got ACK\r\n");
                break;
            }
            //if max retry flag is set
            if(get_max_retry_flag() ) {
                clear_max_retry_flag();
                if(get_plos_count() >=15){
                    retval = -1;
                            set_frequency_offset(2);//to reset PLOS_CNT
                            break;
                }
            }
        }

        RadioMode(MODE_STANDBY);
        //clear data sent flag
        clear_data_sent_flag();

        //if got ack packet, just flush it
        if(get_data_ready_flag()){
            //do what needs to be done with the ACK payload here
            flush_rx();
        }

        //restore original machine state
        RadioMode(originalMode);
        flush_tx();
    }
    
    return retval;
    
}

void NRF24L01p::func(){
   int retval;

    flush_tx();
    flush_rx();


    TxFifo2RadioTxFifo();
    
    
    //backup original machine state
    StateType originalMode = RadioState;

    //has data to write and no data to read. Do not enter PTX with data in PRX payload.
    //This is because, if the PRX is full, then sending a data and will forever wait for ACK
    //and it will never get the ACK because the PRX fifo is full
    if(writable() && !readable()){
        //switching to STANDBY to avoid data reception during state check (ATOMIC state check)
        RadioMode(MODE_STANDBY);

        //clear data sent flag before the next packet is sent
        clear_data_sent_flag();
        //strobe CE for single transmission
        RadioMode(MODE_TX);


        //uint8_t observe;

        while(1){
           //break when DS flag is set and a single payload is sent or all data on RX FIFO sent
            if(get_data_sent_flag() ){
                retval = 0;
                printf("got ACK\r\n");
                break;
            }
            //if max retry flag is set
            if(get_max_retry_flag() ) {
                clear_max_retry_flag();
                if(get_plos_count() >=15){
                    retval = -1;
                            set_frequency_offset(2);//to reset PLOS_CNT
                            break;
                }
            }
        }

        RadioMode(MODE_STANDBY);
        //clear data sent flag
        clear_data_sent_flag();

        //if got ack packet, just flush it
        if(get_data_ready_flag()){
            //do what needs to be done with the ACK payload here
            flush_rx();
        }

        //restore original machine state
        RadioMode(originalMode);
        flush_tx();
    }
    

}