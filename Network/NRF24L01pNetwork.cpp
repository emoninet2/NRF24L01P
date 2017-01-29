/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pNetwork.cpp
 * Author: emon1
 * 
 * Created on January 29, 2017, 12:30 PM
 */

#include "NRF24L01pNetwork.h"

NRF24L01pNetwork::NRF24L01pNetwork() {
}

NRF24L01pNetwork::NRF24L01pNetwork(const NRF24L01pNetwork& orig) {
}

NRF24L01pNetwork::~NRF24L01pNetwork() {
}

void NRF24L01pNetwork::enableBroadcast(bool sel){
    disable_auto_ack(PIPE_P0);
    set_RX_pipe_address((pipe_t) 0,NRF24L01P_NETWORK_BROADCAST_ADDR);
}
int NRF24L01pNetwork::processBroadcastPacket(Payload_t *payload){
    printf("got broadcast packet\r\n");
}
int NRF24L01pNetwork::broadcastPacket(Payload_t *payload){
    payload->TxAddr = NRF24L01P_NETWORK_BROADCAST_ADDR;
    disable_auto_ack(PIPE_P0);
    int ret = TransmitPacket(payload);
    disable_auto_ack(PIPE_P0);
    return ret;
}
int NRF24L01pNetwork::assignToAdjacent(AdjNode_t *node){

}