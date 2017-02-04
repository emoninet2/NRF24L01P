/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NRF24L01p.h
 * Author: emon1
 *
 * Created on January 29, 2017, 7:10 AM
 */

#ifndef NRF24L01P_H
#define NRF24L01P_H

#include "NRF24L01pDriver.h"
#include "NRF24L01pConfig.h"


class NRF24L01p : public NRF24L01pDriver{
public:
    NRF24L01p();
    NRF24L01p(const NRF24L01p& orig);
    virtual ~NRF24L01p();

private:

};

#endif /* NRF24L01P_H */

