#pragma once
#include "RdtSender.h"
#include "stdafx.h"

class SRRdtSender :
    public RdtSender
{
private:
    pair <Packet, bool> SendWnd[SRWndSize + 1]; //窗口记录的是包和包的确认状态. 数组大小为窗口大小+1，可保证wndright指向的是下一个可写位置，但该位置不包含在窗口内。注意当wndleft==wndright表示窗口大小为0。
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

