#include "stdafx.h"
#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver() : ExpectLeftSeq(0), ExpectRightSeq(SRWndSize - 1), wndleft(0) {
	for (auto wndi : RcvWnd) wndi.second = false;	//由于SR接收方可能不按顺序接收，因此bool要初始化
	ackPkt.seqnum = -1;	//忽略该字段
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackPkt.payload[i] = '.';
	}
}

SRRdtReceiver::~SRRdtReceiver() {

}

void SRRdtReceiver::receive(const Packet& packet) {
	int checksum = pUtils->calculateCheckSum(packet);
	bool checkrange1 = ExpectLeftSeq <= ExpectRightSeq && packet.seqnum >= ExpectLeftSeq && packet.seqnum <= ExpectRightSeq;
	bool checkrange2 = ExpectLeftSeq > ExpectRightSeq && (packet.seqnum >= ExpectLeftSeq || packet.seqnum <= ExpectRightSeq);
	if (checksum == packet.checksum && (checkrange1 || checkrange2)) {
		pUtils->printPacket("SR接收方正确接收到报文", packet);
		int pos = (wndleft + (packet.seqnum + (1 << SRSeqBits) - ExpectLeftSeq) % (1 << SRSeqBits))% SRWndSize;	//当前接收包在窗口中的存放位置
		RcvWnd[pos].first = packet;	//记录下当前包
		RcvWnd[pos].second = true;	//并记录有效性
		ackPkt.acknum = packet.seqnum;	//构造ack包
		ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
		pns->sendToNetworkLayer(SENDER, ackPkt);
		pUtils->printPacket("SR接收方发送确认报文", ackPkt);

		if (packet.seqnum == ExpectLeftSeq) {	//判断是否可以向上层批量递交包
			while (RcvWnd[wndleft].second == true) {	//当底部包为窗口内有效包时，递交data并移动窗口
				memcpy(msg.data, RcvWnd[wndleft].first.payload, sizeof(RcvWnd[wndleft].first.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				pUtils->printPacket("SR接收方向应用层递交数据", RcvWnd[wndleft].first);
				RcvWnd[wndleft].second = false;	//移出窗口前将该包标记为无效
				wndleft = (wndleft + 1) % SRWndSize;
				ExpectLeftSeq = (ExpectLeftSeq + 1) % (1 << SRSeqBits);
			}
		}
		printwnd();	//输出当前窗口
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("SR接收方没有正确接收到报文，数据校验出错", packet);
		}
		else {
			pUtils->printPacket("SR接收方没有正确接收到报文，报文序号有误", packet);
			ackPkt.acknum = packet.seqnum;
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("SR接收方重新发送ACK报文", ackPkt);	//对窗口外的包重发确认报文，确保发送方能够移动窗口
			pns->sendToNetworkLayer(SENDER, ackPkt);
		}
	}
	ExpectRightSeq = (ExpectLeftSeq + SRWndSize - 1) % (1 << SRSeqBits);	//更新ExpectRightSeq，注意窗口大小和序列号范围的逻辑
	//cout << "wndleft: " << wndleft << " ExpectLeftSeq: " << ExpectLeftSeq << " ExpectRightSeq: " << ExpectRightSeq << endl;
}

void SRRdtReceiver::printwnd() {
	cout << "SR接收方当前滑动窗口：[ ";
	for (int it = wndleft; it != (wndleft + SRWndSize - 1) %SRWndSize; it = (it + 1) % SRWndSize) {
		if (RcvWnd[it].second) { cout << RcvWnd[it].first.seqnum << " "; }
		else { cout << "*" << " "; }
	}
	cout << "]" << endl;
}