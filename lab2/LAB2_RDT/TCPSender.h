#pragma once
#include "RdtSender.h"
#include "stdafx.h"

class TCPSender :
    public RdtSender
{
private:
    Packet SendWnd[GBNWndSize + 1];   //数组大小为窗口大小+1，可保证wndright指向的是下一个可写位置，但该位置不包含在窗口内。注意当wndleft==wndright表示窗口大小为0
    //map<int, int> find; //seq到数组的映射，可将查找复杂度降低到log
    int expectSeqNumSend;
    int wndleft, wndright;  //指明当前窗口的左右界（数组序号）
    int redundancyack;  //冗余ack计数器
    //pair<int, int> wndleft, wndright;   //first表示在当前窗口中的数组序号，second表示该序号处代表的seq号
    bool WaitingState;
public:
    bool send(const Message& message);
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
    bool getWaitingState();
    void printwnd();
public:
    TCPSender();
    virtual ~TCPSender();
};

