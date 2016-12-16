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
    
    RoutingTableLevel = 0;
}



void NRF24L01pNetwork::processPacket(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;
    
    //printf("your message was : %s\r\n", payload->data);
    //printf("your address was : %llx\r\n", payload.TXaddress);
    //printf("your pipe was : %d\r\n", payload->RXpipe);
    printf("\r\n%d bytes on pipe %d\r\n", strlen((char*)payload->data), payload->RxPipe);
    printf("fromAddr : %x\r\n", network_pld->fromNodeId);
    printf("toAddr : %x\r\n", network_pld->toNodeId);
    printf("pid : %x\r\n", network_pld->pid);
    printf("payload data : %s\r\n", network_pld->payload);

    AdjNode_t returnNode;
    returnNode.RxPipe = payload->RxPipe;
    returnNode.NodeId = AdjacentNodes[returnNode.RxPipe-1].NodeId;
    
    
    RoutingTableHandler(payload);
    
    
    if(ownIdMatched(payload) ){
        printf("wallahi address matched from adj: %x pipe : %d \r\n", returnNode.NodeId,returnNode.RxPipe );
        //sendToNodeSpecific(&ackPld,AdjacentNodes[payload->RxPipe - 1].NodeId);
        network_payload_t ackPld;
        ackPld.toNodeId = network_pld->fromNodeId;
        ackPld.fromNodeId = network_pld->toNodeId;
        ackPld.pid = network_pld->pid;
        ackPld.packet_info = network_pld->packet_info;
        sprintf((char*)ackPld.payload, "ACK");
        xSendToNetworkViaNode(&ackPld, &returnNode);
    }
    else{
        printf("bouncing packet\r\n");
        //sendToNetwork(network_pld);
        xBounceToNetworkExceptNode(network_pld, &returnNode);
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
        bool adjAddrMatch = 0;
        int i;
        for(i=0;i<5;i++){
            if( (Netpayload->toNodeId == AdjacentNodes[i].NodeId  )) {
                    sendToNodeDirect(Netpayload);
                    adjAddrMatch = 1;
                    break;
            }
        }
        if(adjAddrMatch == 0){
            sendToAllAdjacent(Netpayload);
        }
    //}
    return 0;
}
int NRF24L01pNetwork::sendToNetworkViaNode(network_payload_t *Netpayload, uint16_t node){
    //if( isNodeReachable(Netpayload->toNodeId)){
    //	sendToNodeDirect(Netpayload);
    //}
    //else{
        bool adjAddrMatch = 0;
        int i;
        for(i=0;i<5;i++){
            if(node == AdjacentNodes[i].NodeId) {
                sendToNodeDirect(Netpayload);
                adjAddrMatch = 1;
                break;
            }
        }
    //}
    return 0;
}

int NRF24L01pNetwork::sendToNodeDirect(network_payload_t *Netpayload){
    Payload_t payload;
    payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(Netpayload->toNodeId)<<8) + (uint64_t)0xC2;
    strcpy((char*)payload.data, (char*)Netpayload);
    if (fifo_write(&TxFifo, &payload)){
    	return 1;
    }
    else{
    	return 0;
    }
}
int NRF24L01pNetwork::sendToNodeSpecific(network_payload_t *Netpayload, uint16_t node){
    Payload_t payload;
    payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(node)<<8) + (uint64_t)0xC2;
    printf("sending via ADDR : %llx\r\n", payload.TxAddr);
    strcpy((char*)payload.data, (char*)Netpayload);
    if (fifo_write(&TxFifo, &payload)){
    	return 1;
    }
    else{
    	return 0;
    }
}
int NRF24L01pNetwork::sendToAllAdjacent(network_payload_t *Netpayload){

    Payload_t payload;
    int i;
    strcpy((char*)payload.data, (char*)Netpayload);
    for(i=0;i<5;i++){
        payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)0xC2;
        fifo_write(&TxFifo, &payload);
    }
    return 0;
}





void NRF24L01pNetwork::xInit_network(uint16_t networkID, uint16_t nodeID){

}
void NRF24L01pNetwork::xProcessPacket(Payload_t *payload){

}
bool NRF24L01pNetwork::xOwnIdMatched(Payload_t *payload){

}
int NRF24L01pNetwork::xSendToNetworkViaNode(network_payload_t *Netpayload, AdjNode_t *AdjNode){
    Payload_t payload;
    payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjNode->NodeId)<<8) + (uint64_t)(0xC1 +  AdjNode->RxPipe);
    strcpy((char*)payload.data, (char*)Netpayload);
    if (fifo_write(&TxFifo, &payload)){
    	return 1;
    }
    else{
    	return 0;
    }
}
int NRF24L01pNetwork::xBounceToNetworkExceptNode(network_payload_t *Netpayload, AdjNode_t *AdjNode){
    Payload_t payload;
    int i;
    int matched_adjacent = 0;
    int matched_addrCache = 0;
    strcpy((char*)payload.data, (char*)Netpayload);
    for(i=0;i<5;i++){
        if((AdjacentNodes[i].NodeId == Netpayload->toNodeId)&&(AdjacentNodes[i].NodeId != AdjNode->NodeId)){
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
            printf("bouncing to Adjacent Node: %llx\r\n", payload.TxAddr);
            fifo_write(&TxFifo, &payload);
            matched_adjacent = 1;
            return 0;
        }
    }
    
    for(i=0;i<20;i++){
        if((RoutingTable[i].SrcNodeId == Netpayload->toNodeId)){
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(RoutingTable[i].AdjNode.NodeId)<<8) + (uint64_t)(0xC1 + RoutingTable[i].AdjNode.RxPipe);
            printf("bouncing to Routing Table Node: %llx\r\n", payload.TxAddr);
            fifo_write(&TxFifo, &payload);
            matched_adjacent = 1;
            return 0;
        }
    }
    
    
    
    if((matched_adjacent == 0) && (matched_addrCache == 0)){
        for(i=0;i<5;i++){
            payload.TxAddr = ((uint64_t)ownNetworkId<<24) +( (uint64_t)(AdjacentNodes[i].NodeId)<<8) + (uint64_t)(0xC1 + AdjacentNodes[i].RxPipe);
            printf("bouncing to All Adjacent: %llx\r\n", payload.TxAddr);
            fifo_write(&TxFifo, &payload);
            return 0;
        }
    }
    
    return 0;
}

int NRF24L01pNetwork::xIsNodeAdjacent(AdjNode_t AdjNode){

}


int NRF24L01pNetwork::xIsNodeReachable(AdjNode_t AdjNode){

}

int NRF24L01pNetwork::RoutingTableHandler(Payload_t *payload){
    network_payload_t *network_pld = (network_payload_t*) payload->data;

    AdjNode_t returnNode;
    returnNode.RxPipe = payload->RxPipe;
    returnNode.NodeId = AdjacentNodes[returnNode.RxPipe-1].NodeId;
   
    int i;

    for(i=0;i<20;i++){
        if(RoutingTable[i].SrcNodeId == network_pld->fromNodeId) return 0;
    }

    RoutingTable[RoutingTableLevel].SrcNodeId = network_pld->fromNodeId;
    RoutingTable[RoutingTableLevel].AdjNode.RxPipe = payload->RxPipe;
    RoutingTable[RoutingTableLevel].AdjNode.NodeId = AdjacentNodes[returnNode.RxPipe-1].NodeId;
    RoutingTableLevel++;
    if(RoutingTableLevel>=20)RoutingTableLevel = 0;
    
}