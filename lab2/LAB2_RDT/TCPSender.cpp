#include "stdafx.h"
#include "TCPSender.h"
#include "Global.h"

TCPSender::TCPSender() : expectSeqNumSend(0), WaitingState(false), wndleft(0), wndright(0), redundancyack(0) {	//��ʼ��wndrightΪ-1�������ȡ�����У������͵�һ������֮ǰֱ�Ӽ���0��

}
TCPSender::~TCPSender() {

}

bool TCPSender::send(const Message& message) {
	if (WaitingState) return false;	//�����ͷ����ڵȴ�״̬�򷵻�false
	SendWnd[wndright].acknum = -1;	//��ʾ����ack�ֶ�
	SendWnd[wndright].seqnum = expectSeqNumSend;
	expectSeqNumSend = (expectSeqNumSend + 1) % (1 << GBNSeqBits);	//������һ��Ҫ���͵İ��������к�
	memcpy(SendWnd[wndright].payload, message.data, sizeof(message.data));
	SendWnd[wndright].checksum = pUtils->calculateCheckSum(SendWnd[wndright]);
	pUtils->printPacket("TCP���ͷ����ͱ���", SendWnd[wndright]);
	if (wndleft == wndright) { pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndright].seqnum); }	//�Դ����еĵ�һ�������ö�ʱ��
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndright]);
	//cout << wndright << endl;
	wndright = (wndright + 1) % GBNMOD;	//������һ������д��λ��
	printwnd(); //�����ǰ��������
	if (wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1) { WaitingState = true; }	//�������������������ȴ�״̬��Ϊtrue

	return true;
}

void TCPSender::receive(const Packet& ackPkt) {
	if (wndleft == wndright) {	//���ʹ���Ϊ����ֱ�ӷ���
		cout << "TCP���ͷ����ʹ���Ϊ�գ��������ȷ�ϱ���" << endl;
		return;
	}
	int checksum = pUtils->calculateCheckSum(ackPkt);
	//cout << checksum << " " << ackPkt.checksum << " " << wndleft << " " << SendWnd[wndleft].seqnum << " " << wndright << " " << SendWnd[(wndright + GBNMOD - 1) % GBNMOD].seqnum << " " << ackPkt.acknum << endl;
	int leftseq = SendWnd[wndleft].seqnum;
	int rightseq = SendWnd[(wndright + GBNMOD - 1) % GBNMOD].seqnum;
	bool checkrange1 = leftseq <= rightseq && ackPkt.acknum >= leftseq && ackPkt.acknum <= rightseq;	//��������С���Ҳ���ţ��������ж�
	bool checkrange2 = leftseq > rightseq && (ackPkt.acknum <= rightseq || ackPkt.acknum >= leftseq);	//�������Ŵ����Ҳ���ţ����ع��ж�
	//cout << "checkrange: " << checkrange1 << " " << checkrange2 << endl;
	if ((checksum == ackPkt.checksum) && (checkrange1 || checkrange2)) {	//ִ�в�����������checksum���������к�����ȷ�ϴ�����
		//cout << "Correct!" << wndleft << " " << wndright << endl;
		pns->stopTimer(SENDER, SendWnd[wndleft].seqnum);	//ֹͣ��������Ψһ��ʱ��
		//int ackrange = checkrange1 ? ackPkt.acknum - SendWnd[wndleft].seqnum + 1 : ;	//�Ӵ�����࿪ʼȷ�ϵİ��ĸ���
		while (SendWnd[wndleft].seqnum != ackPkt.acknum) {
			pUtils->printPacket("TCP���ͷ���ȷ�յ�ȷ�ϣ������ۻ�ȷ����", SendWnd[wndleft]);
			wndleft = (wndleft + 1) % GBNMOD;
		}
		pUtils->printPacket("TCP���ͷ���ȷ�յ�ȷ�ϣ������ۻ�ȷ����", SendWnd[wndleft]);	//����ȵ�Ҳȷ��
		wndleft = (wndleft + 1) % GBNMOD;
		printwnd(); //�����ǰ��������
		if (wndleft != wndright) { pns->startTimer(SENDER, Configuration::TIME_OUT, SendWnd[wndleft].seqnum); }
	}
	else {
		//cout << wndleft << " " << wndright << endl;
		if (checksum != ackPkt.checksum) {
			cout << "TCP���ͷ��յ���ȷ�ϰ�У������" << endl;
		}
		else { 
			cout << "TCP���ͷ��յ�������ack����ǰ����ack��ĿΪ" << ++redundancyack << endl; 
			if (redundancyack >= 3) {
				redundancyack = 0;
				pns->sendToNetworkLayer(RECEIVER, SendWnd[wndleft]);
				pUtils->printPacket("�յ�3������ack�����ڿ�ʼ�����ش�", SendWnd[wndleft]);
			}
		}	//TCP������ack�Ŀ���
	}
	if (!(wndright == wndleft - 1 || wndright == wndleft + GBNMOD - 1)) { WaitingState = false; }	//ֻҪ�����㴰�������������Ͱѵȴ�״̬��Ϊfalse
}

void TCPSender::timeoutHandler(int seqNum) {
	//���ڼ�ʱ����Ȼֻ�Ǽ�¼�˴�����׵�Ԫ�أ���˲���Ҫ�����ط���ʼ�ĵط�
	//int resendnum = SendWnd[wndright-1].seqnum - seqNum + 1;	//��Ҫ�ش��ĸ���
	//int currresend = (wndright + GBNMOD - resendnum) % TCPMOD;
	pns->stopTimer(SENDER, seqNum);								//������ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	//cout << wndleft << " " << wndright << endl;
	pUtils->printPacket("TCP���ͷ���ʱ��ʱ�䵽����ʼ��ʱ�ش�", SendWnd[wndleft]);
	pns->sendToNetworkLayer(RECEIVER, SendWnd[wndleft]);
}

bool TCPSender::getWaitingState() {
	return WaitingState;
}

void TCPSender::printwnd() {
	cout << "TCP���ͷ���ǰ�������ڣ�[ ";
	for (int it = wndleft; it != wndright; it = (it + 1) % GBNMOD) { cout << SendWnd[it].seqnum << " "; }
	cout << "]" << endl;
}