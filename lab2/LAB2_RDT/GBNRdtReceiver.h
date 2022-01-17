#pragma once
#include "RdtReceiver.h"
#include "stdafx.h"

class GBNRdtReceiver :
    public RdtReceiver
{
private:
    int ExpectSeqNum;      //期待收到的ack
    Packet lastAckPkt;
public:
    GBNRdtReceiver() ;
    virtual ~GBNRdtReceiver();
public:
    void receive(const Packet& packet);
};

