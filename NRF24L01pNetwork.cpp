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
    
    RoutingTableAddr = 0;
}

void NRF24L01pNetwork::setAdjacentNode(pipe_t RxPipe, uint16_t nodeId, pipe_t AdjNodeRxPipe){
    AdjNode[RxPipe-1].NodeId = nodeId;
    AdjNode[RxPipe-1].RxPipe = AdjNodeRxPipe;
    AdjNode[RxPipe-1].enable = 1;
}

void NRF24L01pNetwork::processPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    //printf("your message was : %s\r\n", payload->data);
    //printf("your address was : %llx\r\n", payload.TXaddress);
    //printf("your pipe was : %d\r\n", payload->RxPipe);
    printf("\r\n%d bytes on pipe %d\r\n", strlen((char*)payload->data), payload->RxPipe);
    printf("fromAddr : %x\r\n", network_pld->srcNodeId);
    printf("toAddr : %x\r\n", network_pld->destNodeId);
    printf("pid : %x\r\n", network_pld->pid);
    printf("info : %x\r\n", network_pld->packetInfo);
    printf("payload data : %s\r\n", network_pld->payload);
    
    
    routingTableUpdate(payload);
    
    
    //check if destination is own
    if(network_pld->destNodeId == ownNodeId){
        printf("packet destination matched own ID\r\n");
    }
    else{
        printf("forwarding packet\r\n");
        forwardPacket(payload);
    }
    
    
}

void NRF24L01pNetwork::sendToAdjacent(network_payload_t *NetPayload, AdjNode_t *AdjNode){
        Payload_t payload;
        payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode->NodeId)<<8) + (uint64_t)(0xC0+AdjNode->RxPipe);
        memcpy(payload.data, NetPayload, 32);
        int ret = fifo_write(&TxFifo, &payload);
}

void NRF24L01pNetwork::sendToNode(network_payload_t *NetPayload, uint16_t Node){
        Payload_t payload;
        memcpy(payload.data, NetPayload, 32);
        int i;
        
        printf("checking if node is adjacent\r\n");
        for(i=0;i<5;i++){
            if(Node == AdjNode[i].NodeId){
                payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode[i].NodeId)<<8) + (uint64_t)(0xC0+AdjNode[i].RxPipe);
                printf("node is adjacent. Sending to : %llx\r\n", payload.TxAddr);
                int ret = fifo_write(&TxFifo, &payload);
                return;
            }
        }
        
        printf("checking if node is on Routing Table\r\n");
        for(i=0;i<20;i++){
            if((NetPayload->destNodeId == RoutingTable[i].NodeId)){
                payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(RoutingTable[i].FwrdAdjNode.NodeId)<<8) + (uint64_t)(0xC0+ RoutingTable[i].FwrdAdjNode.RxPipe);
                printf("node is on routing table. sending to : %llx\r\n", payload.TxAddr);
                int ret = fifo_write(&TxFifo, &payload);
                return;
            }
        }
        
        printf("sending to all adjacent\r\n");
        for(i=0;i<5;i++){
            if(AdjNode[i].enable == 1){
                payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode[i].NodeId)<<8) + (uint64_t)(0xC0+AdjNode[i].RxPipe);
                int ret = fifo_write(&TxFifo, &payload);
            }
        }
        
}


void NRF24L01pNetwork::forwardPacket(Payload_t *payload){
    printf("\r\tFORWARDING PACKET\r\n");
    network_payload_t *NetPayload = (network_payload_t*) payload->data;
    Payload_t FwrdPayload;
    AdjNode_t viaNode;
    viaNode.NodeId = AdjNode[payload->RxPipe - 1].NodeId;
    viaNode.RxPipe = AdjNode[payload->RxPipe - 1].RxPipe;
    printf("incoming Node is : %x:%d\r\n", viaNode.NodeId, viaNode.RxPipe);

    memcpy(payload->data, NetPayload, 32);
    int i;

    printf("checking if node is adjacent except return Node\r\n");
    for(i=0;i<5;i++){
        if((NetPayload->destNodeId == AdjNode[i].NodeId) &&(viaNode.NodeId != AdjNode[i].NodeId ) ){
            FwrdPayload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode[i].NodeId)<<8) + (uint64_t)(0xC0+AdjNode[i].RxPipe);
            printf("node is adjacent. bouncing to : %llx\r\n", FwrdPayload.TxAddr);
            int ret = fifo_write(&TxFifo, &FwrdPayload);
            return;
        }
    }

    
    printf("checking if node is on Routing Table\r\n");
    for(i=0;i<20;i++){
        if((NetPayload->destNodeId == RoutingTable[i].NodeId)){
            FwrdPayload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(RoutingTable[i].FwrdAdjNode.NodeId)<<8) + (uint64_t)(0xC0+ RoutingTable[i].FwrdAdjNode.RxPipe);
            printf("node is on routing table. bouncing to : %llx\r\n", FwrdPayload.TxAddr);
            int ret = fifo_write(&TxFifo, &FwrdPayload);
            return;
        }
    }
    
    
    

    printf("bouncing to all adjacent except return Node\r\n");
    for(i=0;i<5;i++){
        if((AdjNode[i].enable == 1)&&(viaNode.NodeId != AdjNode[i].NodeId)){
            FwrdPayload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode[i].NodeId)<<8) + (uint64_t)(0xC0+AdjNode[i].RxPipe);
            printf("bouncing to : %llx\r\n", FwrdPayload.TxAddr);
            int ret = fifo_write(&TxFifo, &FwrdPayload);
        }
    }
}


void NRF24L01pNetwork::routingTableUpdate(Payload_t *payload){
    printf("\r\tROUTING TABLE HANDLER\r\n");
    network_payload_t *NetPayload = (network_payload_t*) payload->data;
    int i;
    
    for(i=0;i<5;i++){
        if((NetPayload->srcNodeId == AdjNode[i].NodeId)  ){
            printf("already Adjacent. Not storing\r\n");
            return;
        }
    }

    for(i=0;i<20;i++){
        if((NetPayload->srcNodeId == RoutingTable[i].NodeId)  ){
            printf("already on routing table. Not storing\r\n");
            return;
        }
    }
    printf("storing to routing table\r\n");
    RoutingTable[RoutingTableAddr].NodeId = NetPayload->srcNodeId;
    RoutingTable[RoutingTableAddr].FwrdAdjNode.NodeId = AdjNode[payload->RxPipe-1].NodeId;
    RoutingTable[RoutingTableAddr].FwrdAdjNode.RxPipe = AdjNode[payload->RxPipe-1].RxPipe;
    RoutingTableAddr++;
    if(RoutingTableAddr>=20) RoutingTableAddr = 0;
    
    printf("New Routing table\r\n");
    for(i=0;i<20;i++){
        printf("\t\t%x --> [%x:%d]\r\n",RoutingTable[i].NodeId,RoutingTable[i].FwrdAdjNode.NodeId,RoutingTable[i].FwrdAdjNode.RxPipe  );
    }
    
}