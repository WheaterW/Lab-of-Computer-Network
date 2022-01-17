#include "stdafx.h"
#include "SRRdtSender.h"
#include "Global.h"

SRRdtSender::SRRdtSender() : expectSeqNumSend(0), WaitingState(false), wndleft(0), wndright(0) {

}

SRRdtSender::~SRRdtSender() {

}

bool SRRdtSender::send(const Message& message) {
	if (WaitingState) { return false; }
	SendWnd[wndright].first.acknum = -1;	//忽略该字段


	SendWnd[wndright].first.seqnum = expectSeqNumSend;
	expectSeqNumSend = (expectSeqNumSend + 1) % (1 << SRSeqBits);
	memcpy(SendWnd[wndright].first.payload, message.data, sizeof(message.data));
	SendWnd[wndright].first.checksum = pUtils->calculateCheckSum(SendWnd[wndright].first);
	SendWnd[wndright].second = false;	//设置该包状态为未确认
	pUtils->printPacket("SR发送方发送报文", SendWnd[wndright].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndright].first.seqnum);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndright].first);
	wndright = (wndright + 1) % SRMOD;
	printwnd(); //输出滑动窗口
	if (wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1) { WaitingState = true; }	//如果满足窗口满的条件，则等待状态设为true
	return true;
}

void SRRdtSender::receive(const Packet& ackPkt) {
	if (wndleft == wndright) {	//发送窗口为空则直接返回
		cout << "SR发送方发送窗口为空，无需接收确认报文" << endl;
		return;
	}
	int checksum = pUtils->calculateCheckSum(ackPkt);
	int leftseq = SendWnd[wndleft].first.seqnum;	//获取当前窗口左侧的序号
	int rightseq = SendWnd[(wndright + SRMOD - 1) % SRMOD].first.seqnum;
	bool checkrange1 = leftseq <= rightseq && ackPkt.acknum >= leftseq && ackPkt.acknum <= rightseq;
	bool checkrange2 = leftseq > rightseq && (ackPkt.acknum >= leftseq || ackPkt.acknum <= rightseq) && ackPkt.acknum >= 0 && ackPkt.acknum <= (1 << SRSeqBits) - 1;
	if ((checksum == ackPkt.checksum) && (checkrange1 || checkrange2)) {
		int ackrange = ackPkt.acknum >= SendWnd[wndleft].first.seqnum ? ackPkt.acknum - SendWnd[wndleft].first.seqnum : ackPkt.acknum + (1 << SRSeqBits) - SendWnd[wndleft].first.seqnum;  //计算seqnum到窗口底部的seqnum的距离
		int ackpos = (wndleft + ackrange) % SRMOD;
		//bool isackd = true;	//记录当前ack包之前的包是否都已经ack
		pns->stopTimer(SENDER, ackPkt.acknum);
		pUtils->printPacket("SR发送方正确收到确认，正在确认中", ackPkt);
		SendWnd[ackpos].second = true;
		if (ackrange == 0) {	//如果当前确认的是最底部的包，向后查看知道找到未确认的包或窗口为空
			for (; wndleft != wndright; wndleft = (wndleft + 1) % SRMOD) {
				if (!SendWnd[wndleft].second) { break; }
			}
		}
		printwnd(); //输出滑动窗口
	}
	else {
		if (checksum != ackPkt.checksum) {
			cout << "SR发送方收到的确认包校验有误" << endl;
		}
		else { cout << "SR发送方收到的确认包序号有误" << endl; }	//GBN如果收到了过期的ack或checksum有误的ack怎么处理？直接超时。
	}
	if (!(wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1)) { WaitingState = false; }	//只要不满足窗口满的条件，就把等待状态设为false
}

void SRRdtSender::timeoutHandler(int seqNum) {
	int range = seqNum >= SendWnd[wndleft].first.seqnum ? seqNum - SendWnd[wndleft].first.seqnum : seqNum + (1 << SRSeqBits) - SendWnd[wndleft].first.seqnum;  //计算seqnum到窗口底部的seqnum的距离
	int pos = (wndleft + range) % SRMOD;
	pns->stopTimer(SENDER, seqNum);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[pos].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pUtils->printPacket("SR发送方定时器时间到，重发超时报文", SendWnd[pos].first);
}

bool SRRdtSender::getWaitingState() {
	return WaitingState;
}

void SRRdtSender::printwnd() {
	cout << "SR发送方当前滑动窗口：[ ";
	for (int it = wndleft; it != wndright; it = (it + 1) % SRMOD) { 
		if (SendWnd[it].second) { cout << "(" << SendWnd[it].first.seqnum << ",已确认)" << " "; }
		else { cout << "(" << SendWnd[it].first.seqnum << ",未确认)" << " "; }
	}
	cout << "]" << endl;
}