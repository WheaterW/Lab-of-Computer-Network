// main.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"

#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include "TCPReceiver.h"
#include "TCPSender.h"

int main(int argc, char* argv[])
{
	string protocol;
	if (argc > 1) protocol = argv[1];
	cout << protocol << endl;
	RdtSender* ps;
	RdtReceiver* pr;

	if (protocol == "TCP") {
		ps = new TCPSender();		//ʵ����TCPSender
		pr = new TCPReceiver();	//ʵ����TCPReceiver
	}
	else if (protocol == "GBN") {
		ps = new GBNRdtSender();		//ʵ����GBNSender
		pr = new GBNRdtReceiver();	//ʵ����GBNReceiver
	}
	else if (protocol == "SR") {
		ps = new SRRdtSender();		//ʵ����SRSender
		pr = new SRRdtReceiver();	//ʵ����SRReceiver
	}
	else {
		ps = new StopWaitRdtSender();
		pr = new StopWaitRdtReceiver();
	}

	pns->setRunMode(RunMode);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(inputpath);
	pns->setOutputFile(outputpath);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;
}


