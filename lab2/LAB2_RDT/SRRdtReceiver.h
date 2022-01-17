#pragma once
#include "RdtReceiver.h"
#include "stdafx.h"

class SRRdtReceiver :
    public RdtReceiver
{
private:
    int ExpectLeftSeq;  //��ǰ�ڴ��յ��ĵײ���ţ���RcvWnd[wndleft]Ӧ�õ�seqnum���������յ��İ��Ƿ����ʹ������ǰ����
    int ExpectRightSeq; //��ǰ�����Ҳ��ڴ�����ţ����ڶԽ��հ��ķ�Χ�ж�
    pair <Packet, bool> RcvWnd[SRWndSize];  //bool��ʾ�Ƿ�Ե�ǰ������Ч����ʱwndleft~wndright������Чλ��wndright����������λ
    Packet ackPkt;  //ack��
    Message msg;    //���ϲ㷢�͵�data
    int wndleft;    //ֻ��¼������࣬�Ҹ�λ��һ����δȷ�ϵİ�
    //int wndleft, wndright;  //�˴���ʵ���߼���wndright = (wndleft + SRWndSize - 1) % SRWndSize�����

public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();
public:
    void receive(const Packet& packet);
    void printwnd();

};

