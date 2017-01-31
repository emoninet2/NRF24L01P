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
    Network_init();
}

NRF24L01pNetwork::NRF24L01pNetwork(const NRF24L01pNetwork& orig) {
}

NRF24L01pNetwork::~NRF24L01pNetwork() {
}

void NRF24L01pNetwork:: Network_init(){
    printf("NETWORK INITIALIZING\r\n");
    //NodeId = 0;
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
    
    printf("\r\n");
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
    
    
    if(message->Cmd == GENERAL_CALL_REPLY){
        printf("\tgot general call\r\n");
        respMesg.Cmd = REPLY_GENERAL_CALL;
        broadcastPacket((Payload_t*)&respMesg);
    }
    else if(message->Cmd == PING_UID){
        if(message->destUID == uid){
            printf("\tgot ping UID match\r\n");
            respMesg.Cmd = PONG_UID;
            broadcastPacket((Payload_t*)&respMesg);
        }
    }
    else if(message->Cmd == REQUEST_CONNECTION){
        if(message->destUID == uid){
            printf("requesting connection\r\n");
            ObtainAddressDhcAdjacent(message);

        }
    }  

    return 0;
}
int NRF24L01pNetwork::broadcastPacket(Payload_t *payload){
    payload->TxAddr = NRF24L01P_NETWORK_BROADCAST_ADDR;
    int ret = TransmitPacket(payload);
    return ret;
}

int NRF24L01pNetwork::adjacentPipeAvailable(){
    int i;
    for(i=0;i<5;i++){
        if(AdjNode[i].connected == 0) return i;
    }
    return -1;
}


uint16_t NRF24L01pNetwork::ObtainAddressDhcAdjacent(BroadcastMessage_t *message){
    printf("gonna assign Node ID for UID : %x\r\n", message->srcUID);
    uint16_t randNodeId = 0;
    
    BroadcastMessage_t respMesg;
    
    
    uint16_t newNodeId = rand() % (65535 + 1 - 1) + 1;
    printf("the new random number is %x\r\n", newNodeId);
    
    
    
    respMesg.Cmd = RESPOND_CONNECTION;
    respMesg.destUID = message->srcUID;
    respMesg.srcUID = uid;
    memcpy((void*) respMesg.data,(void*) &newNodeId, sizeof(newNodeId));
    //respMesg.data[0] = 0x96;
    //respMesg.data[1] = 0xAB;
    broadcastPacket((Payload_t*)&respMesg);
    //printf("\tgonna say free pipe\r\n");
    
    
    return randNodeId;
}


int NRF24L01pNetwork::requestNetworkJoin(){
Payload_t payload;
    BroadcastMessage_t message;
    while(1){//loop until a general call reply is received
        message.destUID = 0;
        message.srcUID = uid;
        message.NetworkID = NetworkId;
        message.Cmd = GENERAL_CALL_REPLY;
        message.len = 32;
        
        broadcastPacket((Payload_t*)&message);
        if((message.Cmd == 0xC2) && message.destUID == uid ){
            break;
        }
        port_DelayMs(1000);
    }
    printf("\tREPLY FOUND : friend : %x\r\n", message.srcUID);
    
    BroadcastMessage_t message2;
    while(1){//loop until a general call reply is received
        message2.destUID = message.srcUID;
        message2.srcUID = uid;
        message2.NetworkID = NetworkId;
        message2.Cmd = REQUEST_CONNECTION;
        message2.len = 32;
        
        broadcastPacket((Payload_t*)&message2);
        if((message2.Cmd == RESPOND_CONNECTION) && message2.destUID == uid ){
            printf(">>>>>>>>>>>yeahh baby\r\n");
            break;
        }
        port_DelayMs(1000);
    }
    uint16_t newNodeId;
    memcpy((void*) &newNodeId, (void*) message2.data, sizeof(newNodeId));
    
    printf("\tFRIEND NODE HAS FREE PIPE AND ASSIGNED NODE IS : %x\r\n", newNodeId);
    
    
    port_DelayMs(5000);
    
}


int NRF24L01pNetwork::assignToAdjacent(AdjNode_t *node){
    
}