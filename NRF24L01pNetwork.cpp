/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pNetwork.cpp
 * Author: emon1
 * 
 * Created on December 13, 2016, 1:18 AM
 */

#include "NRF24L01pNetwork.h"

#include <inttypes.h>
#include <stdint.h>
#include <limits>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

NRF24L01pNetwork::NRF24L01pNetwork() {
}

NRF24L01pNetwork::NRF24L01pNetwork(const NRF24L01pNetwork& orig) {
}

NRF24L01pNetwork::~NRF24L01pNetwork() {
}



void NRF24L01pNetwork::init_network(uint16_t networkID, uint16_t nodeID){
    ownNetworkId = networkID;
    ownNodeId = nodeID;
    set_RX_pipe_address(PIPE_P1, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)0xC2);
}



void NRF24L01pNetwork::processPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    //printf("your message was : %s\r\n", payload->data);
    //printf("your address was : %llx\r\n", payload.TXaddress);
    //printf("your pipe was : %d\r\n", payload->RXpipe);
    printf("\r\n");
    printf("fromAddr : %x\r\n", network_pld->fromNodeId);
    printf("toAddr : %x\r\n", network_pld->toNodeId);
    printf("pid : %x\r\n", network_pld->pid);
    printf("payload data : %s\r\n", network_pld->payload);
    

    if(ownIdMatched(payload) ){
        printf("address matched\r\n");
    }
    else{
        printf("bouncing packet\r\n");
    }
}


bool NRF24L01pNetwork::ownIdMatched(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    if((network_pld->toNodeId) == ownNodeId ){
        return 1;
    }
    else{
        return 0;
    }
}


int NRF24L01pNetwork::sendToNetwork(network_payload_t *Netpayload){
	//if( isNodeReachable(Netpayload->toNodeId)){
	//	sendToNodeDirect(Netpayload);
	//}
	//else{
		sendToAllAdjacent(Netpayload);
	//}
	return 0;
}


int NRF24L01pNetwork::sendToNodeDirect(network_payload_t *Netpayload){

}

int NRF24L01pNetwork::sendToAllAdjacent(network_payload_t *Netpayload){

    Payload_t payload;
    int i;
    strcpy((char*)payload.data, (char*)Netpayload);
    for(i=0;i<5;i++){
        payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(reachable_Nodes[i])<<8) + (uint64_t)0xC2;
        fifo_write(&TxFifo, &payload);
    }
    return 0;
}


bool NRF24L01pNetwork::isNodeReachable(uint16_t NodeId){

}