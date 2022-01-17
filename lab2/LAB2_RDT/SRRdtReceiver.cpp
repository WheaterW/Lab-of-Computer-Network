#include "stdafx.h"
#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver() : ExpectLeftSeq(0), ExpectRightSeq(SRWndSize - 1), wndleft(0) {
	for (auto wndi : RcvWnd) wndi.second = false;	//����SR���շ����ܲ���˳����գ����boolҪ��ʼ��
	ackPkt.seqnum = -1;	//���Ը��ֶ�
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
		pUtils->printPacket("SR���շ���ȷ���յ�����", packet);
		int pos = (wndleft + (packet.seqnum + (1 << SRSeqBits) - ExpectLeftSeq) % (1 << SRSeqBits))% SRWndSize;	//��ǰ���հ��ڴ����еĴ��λ��
		RcvWnd[pos].first = packet;	//��¼�µ�ǰ��
		RcvWnd[pos].second = true;	//����¼��Ч��
		ackPkt.acknum = packet.seqnum;	//����ack��
		ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
		pns->sendToNetworkLayer(SENDER, ackPkt);
		pUtils->printPacket("SR���շ�����ȷ�ϱ���", ackPkt);

		if (packet.seqnum == ExpectLeftSeq) {	//�ж��Ƿ�������ϲ������ݽ���
			while (RcvWnd[wndleft].second == true) {	//���ײ���Ϊ��������Ч��ʱ���ݽ�data���ƶ�����
				memcpy(msg.data, RcvWnd[wndleft].first.payload, sizeof(RcvWnd[wndleft].first.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				pUtils->printPacket("SR���շ���Ӧ�ò�ݽ�����", RcvWnd[wndleft].first);
				RcvWnd[wndleft].second = false;	//�Ƴ�����ǰ���ð����Ϊ��Ч
				wndleft = (wndleft + 1) % SRWndSize;
				ExpectLeftSeq = (ExpectLeftSeq + 1) % (1 << SRSeqBits);
			}
		}
		printwnd();	//�����ǰ����
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("SR���շ�û����ȷ���յ����ģ�����У�����", packet);
		}
		else {
			pUtils->printPacket("SR���շ�û����ȷ���յ����ģ������������", packet);
			ackPkt.acknum = packet.seqnum;
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("SR���շ����·���ACK����", ackPkt);	//�Դ�����İ��ط�ȷ�ϱ��ģ�ȷ�����ͷ��ܹ��ƶ�����
			pns->sendToNetworkLayer(SENDER, ackPkt);
		}
	}
	ExpectRightSeq = (ExpectLeftSeq + SRWndSize - 1) % (1 << SRSeqBits);	//����ExpectRightSeq��ע�ⴰ�ڴ�С�����кŷ�Χ���߼�
	//cout << "wndleft: " << wndleft << " ExpectLeftSeq: " << ExpectLeftSeq << " ExpectRightSeq: " << ExpectRightSeq << endl;
}

void SRRdtReceiver::printwnd() {
	cout << "SR���շ���ǰ�������ڣ�[ ";
	for (int it = wndleft; it != (wndleft + SRWndSize - 1) %SRWndSize; it = (it + 1) % SRWndSize) {
		if (RcvWnd[it].second) { cout << RcvWnd[it].first.seqnum << " "; }
		else { cout << "*" << " "; }
	}
	cout << "]" << endl;
}