#include "stdafx.h"
#if METHOD == GBN

#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"

int main(int argc, char* argv[])
{
	RdtSender* ps = new GBNRdtSender();		//实例化GBNSender
	RdtReceiver* pr = new GBNRdtReceiver();	//实例化GBNReceiver
	pns->setRunMode(RunMode);  //运行模式，在stdafx中修改
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
#endif // METHOD == GBN
