#include "stdafx.h"
#include "SRRdtSender.h"
#include "Global.h"

SRRdtSender::SRRdtSender() : expectSeqNumSend(0), WaitingState(false), wndleft(0), wndright(0) {

}

SRRdtSender::~SRRdtSender() {

}

bool SRRdtSender::send(const Message& message) {
	if (WaitingState) { return false; }
	SendWnd[wndright].first.acknum = -1;	//���Ը��ֶ�


	SendWnd[wndright].first.seqnum = expectSeqNumSend;
	expectSeqNumSend = (expectSeqNumSend + 1) % (1 << SRSeqBits);
	memcpy(SendWnd[wndright].first.payload, message.data, sizeof(message.data));
	SendWnd[wndright].first.checksum = pUtils->calculateCheckSum(SendWnd[wndright].first);
	SendWnd[wndright].second = false;	//���øð�״̬Ϊδȷ��
	pUtils->printPacket("SR���ͷ����ͱ���", SendWnd[wndright].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndright].first.seqnum);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndright].first);
	wndright = (wndright + 1) % SRMOD;
	printwnd(); //�����������
	if (wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1) { WaitingState = true; }	//������㴰��������������ȴ�״̬��Ϊtrue
	return true;
}

void SRRdtSender::receive(const Packet& ackPkt) {
	if (wndleft == wndright) {	//���ʹ���Ϊ����ֱ�ӷ���
		cout << "SR���ͷ����ʹ���Ϊ�գ��������ȷ�ϱ���" << endl;
		return;
	}
	int checksum = pUtils->calculateCheckSum(ackPkt);
	int leftseq = SendWnd[wndleft].first.seqnum;	//��ȡ��ǰ�����������
	int rightseq = SendWnd[(wndright + SRMOD - 1) % SRMOD].first.seqnum;
	bool checkrange1 = leftseq <= rightseq && ackPkt.acknum >= leftseq && ackPkt.acknum <= rightseq;
	bool checkrange2 = leftseq > rightseq && (ackPkt.acknum >= leftseq || ackPkt.acknum <= rightseq) && ackPkt.acknum >= 0 && ackPkt.acknum <= (1 << SRSeqBits) - 1;
	if ((checksum == ackPkt.checksum) && (checkrange1 || checkrange2)) {
		int ackrange = ackPkt.acknum >= SendWnd[wndleft].first.seqnum ? ackPkt.acknum - SendWnd[wndleft].first.seqnum : ackPkt.acknum + (1 << SRSeqBits) - SendWnd[wndleft].first.seqnum;  //����seqnum�����ڵײ���seqnum�ľ���
		int ackpos = (wndleft + ackrange) % SRMOD;
		//bool isackd = true;	//��¼��ǰack��֮ǰ�İ��Ƿ��Ѿ�ack
		pns->stopTimer(SENDER, ackPkt.acknum);
		pUtils->printPacket("SR���ͷ���ȷ�յ�ȷ�ϣ�����ȷ����", ackPkt);
		SendWnd[ackpos].second = true;
		if (ackrange == 0) {	//�����ǰȷ�ϵ�����ײ��İ������鿴֪���ҵ�δȷ�ϵİ��򴰿�Ϊ��
			for (; wndleft != wndright; wndleft = (wndleft + 1) % SRMOD) {
				if (!SendWnd[wndleft].second) { break; }
			}
		}
		printwnd(); //�����������
	}
	else {
		if (checksum != ackPkt.checksum) {
			cout << "SR���ͷ��յ���ȷ�ϰ�У������" << endl;
		}
		else { cout << "SR���ͷ��յ���ȷ�ϰ��������" << endl; }	//GBN����յ��˹��ڵ�ack��checksum�����ack��ô����ֱ�ӳ�ʱ��
	}
	if (!(wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1)) { WaitingState = false; }	//ֻҪ�����㴰�������������Ͱѵȴ�״̬��Ϊfalse
}

void SRRdtSender::timeoutHandler(int seqNum) {
	int range = seqNum >= SendWnd[wndleft].first.seqnum ? seqNum - SendWnd[wndleft].first.seqnum : seqNum + (1 << SRSeqBits) - SendWnd[wndleft].first.seqnum;  //����seqnum�����ڵײ���seqnum�ľ���
	int pos = (wndleft + range) % SRMOD;
	pns->stopTimer(SENDER, seqNum);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[pos].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pUtils->printPacket("SR���ͷ���ʱ��ʱ�䵽���ط���ʱ����", SendWnd[pos].first);
}

bool SRRdtSender::getWaitingState() {
	return WaitingState;
}

void SRRdtSender::printwnd() {
	cout << "SR���ͷ���ǰ�������ڣ�[ ";
	for (int it = wndleft; it != wndright; it = (it + 1) % SRMOD) { 
		if (SendWnd[it].second) { cout << "(" << SendWnd[it].first.seqnum << ",��ȷ��)" << " "; }
		else { cout << "(" << SendWnd[it].first.seqnum << ",δȷ��)" << " "; }
	}
	cout << "]" << endl;
}