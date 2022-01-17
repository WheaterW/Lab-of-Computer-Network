#include "stdafx.h"
#include "TCPReceiver.h"
#include "Global.h"


TCPReceiver::TCPReceiver() : ExpectSeqNum(0) {
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
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
		pUtils->printPacket("TCP接收方接收到正确报文", packet);

		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		lastAckPkt.acknum = packet.seqnum;
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("TCP接收方发送ACK报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
	else {
		if (checksum != packet.checksum) {
			pUtils->printPacket("TCP接收方没有正确接收到报文，数据校验出错", packet);
		}
		else {
			pUtils->printPacket("TCP接收方没有正确接收到报文，报文序号有误", packet);
		}
		pUtils->printPacket("TCP接收方重新发送上次ACK报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
}
