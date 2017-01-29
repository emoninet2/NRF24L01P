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
void NRF24L01pNetwork::setUID(uint32_t val){
    uid = val;
}

void NRF24L01pNetwork::NetworkUID(uint32_t id){
    NetworkId = id;
}
void NRF24L01pNetwork::enableBroadcast(bool sel){
    set_RX_pipe_address((pipe_t) 0,NRF24L01P_NETWORK_BROADCAST_ADDR);
}
int NRF24L01pNetwork::processBroadcastPacket(Payload_t *payload){
    printf("got broadcast packet\r\n");
    BroadcastMessage_t *message = (BroadcastMessage_t*)payload;
    
    int i;
    for(i=0;i<32;i++){
        printf("%x ", payload->data[i]);
    }
    printf("\r\n");
    
    printf("srcUID : %x\r\n", message->srcUID);
    printf("destUID : %x\r\n", message->destUID);
    printf("NetworkId : %x\r\n", message->NetworkID);
    printf("Command : %x\r\n", message->Cmd);
    
    BroadcastMessage_t respMesg ;
    respMesg.destUID = message->srcUID;
    respMesg.srcUID = uid;
    
    switch(message->Cmd){
        case GENERAL_CALL_REPLY : {
                respMesg.Cmd = REPLY_GENERAL_CALL;
                broadcastPacket((Payload_t*)&respMesg);
            break;
        }
    }
    
    
}
int NRF24L01pNetwork::broadcastPacket(Payload_t *payload){
    payload->TxAddr = NRF24L01P_NETWORK_BROADCAST_ADDR;
    int ret = TransmitPacket(payload);
    return ret;
}

int NRF24L01pNetwork::requestNetworkJoin(){
    BroadcastMessage_t message;
    Payload_t payload;

    message.destUID = 0xABACADAE;
    message.srcUID = uid;
    message.NetworkID = NetworkId;
    message.Cmd = GENERAL_CALL_REPLY;
    message.len = 0x45;
    
    
    memcpy((void*) &payload.data, (void*) &message, 32);
    broadcastPacket(&payload);
    printf("got ack and the len is : %d\r\n", payload.len);
    int i;
    for(i=0;i<32;i++){
        printf("%x ", payload.data[i]);
    }
    printf("\r\n");
}


int NRF24L01pNetwork::assignToAdjacent(AdjNode_t *node){
    
}