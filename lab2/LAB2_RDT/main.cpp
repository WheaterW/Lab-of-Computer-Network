// main.cpp : 定义控制台应用程序的入口点。
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
		ps = new TCPSender();		//实例化TCPSender
		pr = new TCPReceiver();	//实例化TCPReceiver
	}
	else if (protocol == "GBN") {
		ps = new GBNRdtSender();		//实例化GBNSender
		pr = new GBNRdtReceiver();	//实例化GBNReceiver
	}
	else if (protocol == "SR") {
		ps = new SRRdtSender();		//实例化SRSender
		pr = new SRRdtReceiver();	//实例化SRReceiver
	}
	else {
		ps = new StopWaitRdtSender();
		pr = new StopWaitRdtReceiver();
	}

	pns->setRunMode(RunMode);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(inputpath);
	pns->setOutputFile(outputpath);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete

	return 0;
}


