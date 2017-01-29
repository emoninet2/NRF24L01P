/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01pNetwork.h
 * Author: emon1
 *
 * Created on January 29, 2017, 12:30 PM
 */

#ifndef NRF24L01PNETWORK_H
#define NRF24L01PNETWORK_H

#include "../NRF24L01p.h"
#include "NRF24L01pNetworkConfig.h"


class NRF24L01pNetwork : public NRF24L01p{
public:
    typedef enum{
        GENERAL_BROADCAST = 0xC0,
        GENERAL_CALL_REPLY = 0xC1,
        REPLY_GENERAL_CALL = 0xC2,
        PING_UID = 0xC3,
        PONG_UID = 0xC4,
        REQUEST_CONNECTION = 0xC5,
        RESPOND_CONNECTION = 0xC6,          
    }BroadcastCommand_t;
    
    typedef struct networkPayload{
        uint16_t srcNodeId;
        uint16_t destNodeId;
        uint8_t pid;
        uint8_t packetControl;
        uint8_t payload[26];
    }networkPayload_t;
    
    typedef struct AdjNode{
        uint16_t NodeId;
        pipe_t RxPipe;
        bool connected;
    }AdjNode_t;
    
    typedef struct RoutingNode{
        uint16_t NodeId;
        AdjNode_t FwrdAdjNode;
        unsigned int PacketCount;
    }RoutingNode_t;
    
    typedef struct{
        uint32_t srcUID;
        uint32_t destUID;
        uint16_t NetworkID;
        BroadcastCommand_t Cmd; 
        uint8_t len;
        uint8_t data[25];
    }BroadcastMessage_t;
    
    

    
    AdjNode_t AdjNode[5];
    RoutingNode_t RoutingTable[20];

    uint32_t uid;
    uint16_t NetworkId;
    uint16_t NodeId;
    
    
    NRF24L01pNetwork();
    NRF24L01pNetwork(const NRF24L01pNetwork& orig);
    virtual ~NRF24L01pNetwork();
    
    void setUID(uint32_t uid);
    void NetworkUID(uint32_t id);
    void enableBroadcast(bool sel);
    int processBroadcastPacket(Payload_t *payload);
    int broadcastPacket(Payload_t *payload);
    
    
    int requestNetworkJoin();
    int assignToAdjacent(AdjNode_t *node);
    
    
    
    
private:

};

#endif /* NRF24L01PNETWORK_H */

