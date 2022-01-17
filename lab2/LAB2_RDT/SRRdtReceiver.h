#pragma once
#include "RdtReceiver.h"
#include "stdafx.h"

class SRRdtReceiver :
    public RdtReceiver
{
private:
    int ExpectLeftSeq;  //当前期待收到的底部序号，即RcvWnd[wndleft]应该的seqnum，决定了收到的包是否可以使窗口向前滑动
    int ExpectRightSeq; //当前窗口右侧期待的序号，用于对接收包的范围判断
    pair <Packet, bool> RcvWnd[SRWndSize];  //bool表示是否对当前窗口有效，此时wndleft~wndright都是有效位，wndright不再是引导位
    Packet ackPkt;  //ack包
    Message msg;    //向上层发送的data
    int wndleft;    //只记录窗口左侧，且该位置一定是未确认的包
    //int wndleft, wndright;  //此处的实现逻辑是wndright = (wndleft + SRWndSize - 1) % SRWndSize恒成立

public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();
public:
    void receive(const Packet& packet);
    void printwnd();

};

