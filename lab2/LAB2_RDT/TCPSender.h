#pragma once
#include "RdtSender.h"
#include "stdafx.h"

class TCPSender :
    public RdtSender
{
private:
    Packet SendWnd[GBNWndSize + 1];   //�����СΪ���ڴ�С+1���ɱ�֤wndrightָ�������һ����дλ�ã�����λ�ò������ڴ����ڡ�ע�⵱wndleft==wndright��ʾ���ڴ�СΪ0
    //map<int, int> find; //seq�������ӳ�䣬�ɽ����Ҹ��ӶȽ��͵�log
    int expectSeqNumSend;
    int wndleft, wndright;  //ָ����ǰ���ڵ����ҽ磨������ţ�
    int redundancyack;  //����ack������
    //pair<int, int> wndleft, wndright;   //first��ʾ�ڵ�ǰ�����е�������ţ�second��ʾ����Ŵ������seq��
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

