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
    set_RX_pipe_address(PIPE_P1, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC1 + PIPE_P1));
    set_RX_pipe_address(PIPE_P2, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC1 + PIPE_P2));
    set_RX_pipe_address(PIPE_P3, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC1 + PIPE_P3));
    set_RX_pipe_address(PIPE_P4, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC1 + PIPE_P4));
    set_RX_pipe_address(PIPE_P5, ((uint64_t)ownNetworkId<<24) +( (uint64_t)(ownNodeId)<<8) + (uint64_t)(0xC1 + PIPE_P5));
    
    RoutingTablePtrLvl = 0;
}



void NRF24L01pNetwork::processPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
 
    printf("\r\n%d bytes on pipe %d\r\n", strlen((char*)payload->data), payload->RxPipe);
    
    printf("fromAddr : %x\r\n", network_pld->fromNodeId);
    printf("toAddr : %x\r\n", network_pld->toNodeId);
    printf("pid : %x\r\n", network_pld->pid);
    printf("payload data : %s\r\n", network_pld->payload);

    printf("adjacent node : [%x:%d]\r\n", AdjacentNodes[payload->RxPipe-1].NodeId,AdjacentNodes[payload->RxPipe-1].RxPipe );
    
    processRoutingTable(payload);
    
    
    if(network_pld->toNodeId == ownNodeId){
        printf("packet matched own address\r\n");

    }
    else{
        printf("forwarding packet\r\n");
        forwardNetPayloadExceptAdjNode(network_pld, AdjacentNodes[payload->RxPipe-1].NodeId);
    }
    
    
    
    
    

}

void NRF24L01pNetwork::processRoutingTable(Payload_t *payload){
    printf("\t\trouting table handler\r\n");
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    
    int i;
    
    //check if destination node is adjacent node
    for(i=0;i<5;i++){
        if(network_pld->fromNodeId == AdjacentNodes[i].NodeId){
            printf("not storing on routing table. is adjacent\r\n");
            return;
        }
    }
    
    //check if forwarding node information is available on RoutingTable
    for(i=0;i<NRF24L01P_NETWORK_ROUTING_TABLE_SIZE;i++){
        if(network_pld->fromNodeId == RoutingTable[i].SrcNodeId){
            printf("not storing on routing table. is on routing table\r\n");
            return;
        }
    }
    
    Node_t viaNode;
    viaNode.NodeId = AdjacentNodes[payload->RxPipe - 1].NodeId;
    viaNode.RxPipe = AdjacentNodes[payload->RxPipe - 1].RxPipe;        
    
    printf("storing routing information\r\n  %x-->[%x:%d]\r\n", network_pld->fromNodeId, viaNode.NodeId,viaNode.RxPipe );
    
    RoutingTable[RoutingTablePtrLvl].SrcNodeId = network_pld->fromNodeId;
    RoutingTable[RoutingTablePtrLvl].AdjNode.NodeId = viaNode.NodeId;
    RoutingTable[RoutingTablePtrLvl].AdjNode.RxPipe = viaNode.RxPipe;
}




void NRF24L01pNetwork::sendNetPayload(network_payload_t *netPayload){
    printf("\r\rsending payload\r\n");
    
    Payload_t payload;
    strcpy((char*)payload.data, (char*)netPayload);
    int i;
    
    printf("checking if destination is adjacent\r\n");
    //check if destination is adjacent
    for(i=0;i<5;i++){
        if(AdjacentNodes[i].NodeId == netPayload->toNodeId){
            printf("destination is adjacent\r\n");
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
            fifo_write(&TxFifo, &payload);
            return;
        }
    }
    
    printf("checking if destination is on routing table\r\n");
    //check if destination is on routing table
    for(i=0;i<20;i++){
        if(RoutingTable[i].SrcNodeId == netPayload->toNodeId){
            printf("destination is found on routing table\r\n");
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(RoutingTable[i].AdjNode.NodeId)<<8) + (uint64_t)(0xC1 + RoutingTable[i].AdjNode.RxPipe);
            fifo_write(&TxFifo, &payload);
            return;
        }
    }
    
    printf("sending to all adjacent\r\n");
    //send to all adjacent
    for(i=0;i<5;i++){
        payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
        printf("\t\t%llx\r\n", payload.TxAddr);
        fifo_write(&TxFifo, &payload);
    }

}


void NRF24L01pNetwork::forwardNetPayloadExceptAdjNode(network_payload_t *netPayload, uint16_t adjNode){
     printf("\r\rforwarding payload\r\n");
    Payload_t payload;
    strcpy((char*)payload.data, (char*)netPayload);
    int i;
    
    printf("checking if destination is adjacent\r\n");
    //check if destination is adjacent
    for(i=0;i<5;i++){
        if((AdjacentNodes[i].NodeId == netPayload->toNodeId) && AdjacentNodes[i].NodeId != adjNode ){
            printf("destination is adjacent\r\n");
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
            fifo_write(&TxFifo, &payload);
            return;
        }
    }
    
    printf("checking if destination is on routing table\r\n");
    //check if destination is on routing table
    for(i=0;i<20;i++){
        if(RoutingTable[i].SrcNodeId == netPayload->toNodeId){
            printf("destination is found on routing table\r\n");
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(RoutingTable[i].AdjNode.NodeId)<<8) + (uint64_t)(0xC1 + RoutingTable[i].AdjNode.RxPipe);
            fifo_write(&TxFifo, &payload);
            return;
        }
    }
    
    printf("sending to all adjacent except return\r\n");
    //send to all adjacent
    for(i=0;i<5;i++){
        if(AdjacentNodes[i].NodeId != adjNode ){
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
            printf("\t\t%llx\r\n", payload.TxAddr);
            fifo_write(&TxFifo, &payload);
        }

    }
    
    
    
    
}