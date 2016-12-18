/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pNetwork.h
 * Author: emon1
 *
 * Created on December 13, 2016, 1:18 AM
 */

#ifndef NRF24L01PNETWORK_H
#define NRF24L01PNETWORK_H

#include "NRF24L01p.h"

#include <inttypes.h>
#include <stdint.h>
#include <limits>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

class NRF24L01pNetwork : public NRF24L01p{
public:
    typedef struct network_payload{
        uint16_t srcNodeId;
        uint16_t destNodeId;
        uint8_t pid;
        uint8_t packetInfo;
        uint8_t payload[26];
    }network_payload_t;
    
    typedef struct AdjNode{
        uint16_t NodeId;
        pipe_t RxPipe;
        bool enable;
    }AdjNode_t;
    
    typedef struct ForwardingNode{
        uint16_t NodeId;
        AdjNode_t AdjNode;
    }FrwdNode_t;

    
    typedef struct RoutingNode{
        uint16_t NodeId;
        AdjNode_t FwrdAdjNode;
    }RoutingNode_t;
    
    
    
    AdjNode_t AdjNode[5];
   RoutingNode RoutingTable[20];
   unsigned int RoutingTableAddr;
    
    uint16_t ownNetworkId;
    uint16_t ownNodeId;
 
    NRF24L01pNetwork();
    NRF24L01pNetwork(const NRF24L01pNetwork& orig);
    virtual ~NRF24L01pNetwork();

    void init_network(uint16_t networkID, uint16_t nodeID);
    void setAdjacentNode(pipe_t RxPipe, uint16_t nodeId, pipe_t AdjNodeRxPipe);
    void processPacket(Payload_t *payload);
    
    void sendToAdjacent(network_payload_t *payload, AdjNode_t *AdjNode);
    void sendToNode(network_payload_t *NetPayload, uint16_t Node);
    void forwardPacket(Payload_t *payload);
    
    void routingTableUpdate(Payload_t *payload);
    
    
    
private:

};

#endif /* NRF24L01PNETWORK_H */

