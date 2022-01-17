#pragma once
#include "RdtReceiver.h"
#include "stdafx.h"


class TCPReceiver :
    public RdtReceiver
{
private:
    int ExpectSeqNum;      //�ڴ��յ���ack
    Packet lastAckPkt;
public:
    TCPReceiver();
    virtual ~TCPReceiver();
public:
    void receive(const Packet& packet);
};

