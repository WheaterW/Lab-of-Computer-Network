#include "stdafx.h"
#include "TCPSender.h"
#include "Global.h"

TCPSender::TCPSender() : expectSeqNumSend(0), WaitingState(false), wndleft(0), wndright(0), redundancyack(0) {	//初始化wndright为-1，则可以取消特判，即发送第一个包裹之前直接加至0；

}
TCPSender::~TCPSender() {

}

bool TCPSender::send(const Message& message) {
	if (WaitingState) return false;	//若发送方处于等待状态则返回false
	SendWnd[wndright].acknum = -1;	//表示忽略ack字段
	SendWnd[wndright].seqnum = expectSeqNumSend;
	expectSeqNumSend = (expectSeqNumSend + 1) % (1 << GBNSeqBits);	//计算下一个要发送的包裹的序列号
	memcpy(SendWnd[wndright].payload, message.data, sizeof(message.data));
	SendWnd[wndright].checksum = pUtils->calculateCheckSum(SendWnd[wndright]);
	pUtils->printPacket("TCP发送方发送报文", SendWnd[wndright]);
	if (wndleft == wndright) { pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndright].seqnum); }	//对窗口中的第一个包设置定时器
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndright]);
	//cout << wndright << endl;
	wndright = (wndright + 1) % GBNMOD;	//更新下一个“可写”位置
	printwnd(); //输出当前滑动窗口
	if (wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1) { WaitingState = true; }	//如果满足满的条件，则等待状态设为true

	return true;
}

void TCPSender::receive(const Packet& ackPkt) {
	if (wndleft == wndright) {	//发送窗口为空则直接返回
		cout << "TCP发送方发送窗口为空，无需接收确认报文" << endl;
		return;
	}
	int checksum = pUtils->calculateCheckSum(ackPkt);
	//cout << checksum << " " << ackPkt.checksum << " " << wndleft << " " << SendWnd[wndleft].seqnum << " " << wndright << " " << SendWnd[(wndright + GBNMOD - 1) % GBNMOD].seqnum << " " << ackPkt.acknum << endl;
	int leftseq = SendWnd[wndleft].seqnum;
	int rightseq = SendWnd[(wndright + GBNMOD - 1) % GBNMOD].seqnum;
	bool checkrange1 = leftseq <= rightseq && ackPkt.acknum >= leftseq && ackPkt.acknum <= rightseq;	//若左侧序号小于右侧序号，即正常判断
	bool checkrange2 = leftseq > rightseq && (ackPkt.acknum <= rightseq || ackPkt.acknum >= leftseq);	//若左侧序号大于右侧序号，即回滚判断
	//cout << "checkrange: " << checkrange1 << " " << checkrange2 << endl;
	if ((checksum == ackPkt.checksum) && (checkrange1 || checkrange2)) {	//执行操作的条件是checksum无误且序列号是在确认窗口里
		//cout << "Correct!" << wndleft << " " << wndright << endl;
		pns->stopTimer(SENDER, SendWnd[wndleft].seqnum);	//停止窗口左侧的唯一计时器
		//int ackrange = checkrange1 ? ackPkt.acknum - SendWnd[wndleft].seqnum + 1 : ;	//从窗口左侧开始确认的包的个数
		while (SendWnd[wndleft].seqnum != ackPkt.acknum) {
			pUtils->printPacket("TCP发送方正确收到确认，正在累积确认中", SendWnd[wndleft]);
			wndleft = (wndleft + 1) % GBNMOD;
		}
		pUtils->printPacket("TCP发送方正确收到确认，正在累积确认中", SendWnd[wndleft]);	//把相等的也确认
		wndleft = (wndleft + 1) % GBNMOD;
		printwnd(); //输出当前滑动窗口
		if (wndleft != wndright) { pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndleft].seqnum); }
	}
	else {
		//cout << wndleft << " " << wndright << endl;
		if (checksum != ackPkt.checksum) {
			cout << "TCP发送方收到的确认包校验有误" << endl;
		}
		else { 
			cout << "TCP发送方收到的冗余ack，当前冗余ack数目为" << ++redundancyack << endl; 
			if (redundancyack >= 3) {
				redundancyack = 0;
				pns->sendToNetworkLayer(RECEIVER, SendWnd[wndleft]);
				pUtils->printPacket("收到3个冗余ack，正在开始快速重传", SendWnd[wndleft]);
			}
		}	//TCP对冗余ack的控制
	}
	if (!(wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1)) { WaitingState = false; }	//只要不满足窗口满的条件，就把等待状态设为false
}

void TCPSender::timeoutHandler(int seqNum) {
	//由于计时器必然只是记录了窗口最底的元素，因此不需要计算重发开始的地方
	//int resendnum = SendWnd[wndright-1].seqnum - seqNum + 1;	//需要重传的个数
	//int currresend = (wndright + GBNMOD - resendnum) % TCPMOD;
	pns->stopTimer(SENDER, seqNum);								//重启计时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	//cout << wndleft << " " << wndright << endl;
	pUtils->printPacket("TCP发送方定时器时间到，开始超时重传", SendWnd[wndleft]);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndleft]);
}

bool TCPSender::getWaitingState() {
	return WaitingState;
}

void TCPSender::printwnd() {
	cout << "TCP发送方当前滑动窗口：[ ";
	for (int it = wndleft; it != wndright; it = (it + 1) % GBNMOD) { cout << SendWnd[it].seqnum << " "; }
	cout << "]" << endl;
}