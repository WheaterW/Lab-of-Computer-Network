#include "stdafx.h"
#include "TCPReceiver.h"
#include "Global.h"


TCPReceiver::TCPReceiver() : ExpectSeqNum(0) {
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

TCPReceiver::~TCPReceiver() {

}

void TCPReceiver::receive(const Packet& packet) {
	int checksum = pUtils->calculateCheckSum(packet);
	//cout << "Receiver expect: " << ExpectSeqNum << endl;
	if (checksum == packet.checksum && packet.seqnum == ExpectSeqNum) {
		ExpectSeqNum = (ExpectSeqNum + 1) % (1 << GBNSeqBits);
		pUtils->printPacket("TCP���շ����յ���ȷ����", packet);

		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		lastAckPkt.acknum = packet.seqnum;
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("TCP���շ�����ACK����", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("TCP���շ�û����ȷ���յ����ģ�����У�����", packet);
		}
		else {
			pUtils->printPacket("TCP���շ�û����ȷ���յ����ģ������������", packet);
		}
		pUtils->printPacket("TCP���շ����·����ϴ�ACK����", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
}
