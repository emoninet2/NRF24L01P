/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF2401pNetwork.h
 * Author: emon1
 *
 * Created on December 12, 2016, 11:02 PM
 */

#ifndef NRF2401PNETWORK_H
#define NRF2401PNETWORK_H

#include "NRF24L01p.h"


class NRF2401pNetwork : public NRF24L01p{
public:
    NRF2401pNetwork();
    NRF2401pNetwork(const NRF2401pNetwork& orig);
    virtual ~NRF2401pNetwork();
private:

};

#endif /* NRF2401PNETWORK_H */

