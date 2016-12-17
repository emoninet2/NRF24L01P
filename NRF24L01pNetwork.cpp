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
    set_RX_pipe_address(PIPE_P1, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC0 | PIPE_P1));
    set_RX_pipe_address(PIPE_P2, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC0 | PIPE_P2));
    set_RX_pipe_address(PIPE_P3, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC0 | PIPE_P3));
    set_RX_pipe_address(PIPE_P4, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC0 | PIPE_P4));
    set_RX_pipe_address(PIPE_P5, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC0 | PIPE_P5));
    
}

void NRF24L01pNetwork::setAdjacentNode(pipe_t RxPipe, uint16_t nodeId, pipe_t AdjNodeRxPipe){
    AdjNode[RxPipe-1].NodeId = nodeId;
    AdjNode[RxPipe-1].RxPipe = AdjNodeRxPipe;
}

void NRF24L01pNetwork::processPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    printf("your message was : %s\r\n", payload->data);
    //printf("your address was : %llx\r\n", payload.TXaddress);
    printf("your pipe was : %d\r\n", payload->RxPipe);
    printf("\r\n%d bytes on pipe %d\r\n", strlen((char*)payload->data), payload->RxPipe);
    printf("fromAddr : %x\r\n", network_pld->srcNodeId);
    printf("toAddr : %x\r\n", network_pld->destNodeId);
    printf("pid : %x\r\n", network_pld->pid);
    printf("info : %x\r\n", network_pld->packetInfo);
    printf("payload data : %s\r\n", network_pld->payload);
    
    //check if destination is own
    if(network_pld->destNodeId == ownNodeId){
        printf("packet destination matched own ID\r\n");
    }
    else{
        printf("forwarding packet");
    }
    
    
}

void NRF24L01pNetwork::forwardPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    
    
    
}