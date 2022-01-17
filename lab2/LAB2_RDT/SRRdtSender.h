#pragma once
#include "RdtSender.h"
#include "stdafx.h"

class SRRdtSender :
    public RdtSender
{
private:
    pair <Packet, bool> SendWnd[SRWndSize + 1]; //���ڼ�¼���ǰ��Ͱ���ȷ��״̬. �����СΪ���ڴ�С+1���ɱ�֤wndrightָ�������һ����дλ�ã�����λ�ò������ڴ����ڡ�ע�⵱wndleft==wndright��ʾ���ڴ�СΪ0��
    int expectSeqNumSend;
    int wndleft, wndright;
    bool WaitingState;
public:
    bool send(const Message& message);
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
    bool getWaitingState();
    void printwnd();
public:
    SRRdtSender();
    virtual ~SRRdtSender();


};

