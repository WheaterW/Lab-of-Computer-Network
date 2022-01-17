#include "stdafx.h"
#include "GBNRdtReceiver.h"
#include "Global.h"

GBNRdtReceiver::GBNRdtReceiver() : ExpectSeqNum(0) {
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

GBNRdtReceiver::~GBNRdtReceiver() {

}

void GBNRdtReceiver::receive(const Packet& packet) {
	int checksum = pUtils->calculateCheckSum(packet);
	//cout << "Receiver expect: " << ExpectSeqNum << endl;
	if (checksum == packet.checksum && packet.seqnum == ExpectSeqNum) {
		ExpectSeqNum = (ExpectSeqNum + 1) % (1 << GBNSeqBits);
		pUtils->printPacket("GBN���շ����յ���ȷ����", packet);

		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		lastAckPkt.acknum = packet.seqnum;
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("GBN���շ�����ACK����", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("GBN���շ�û����ȷ���յ����ģ�����У�����", packet);
		}
		else {
			pUtils->printPacket("GBN���շ�û����ȷ���յ����ģ������������", packet);
		}
		pUtils->printPacket("GBN���շ����·����ϴ�ACK����", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
}
