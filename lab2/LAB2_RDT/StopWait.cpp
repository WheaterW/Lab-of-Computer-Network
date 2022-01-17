// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h"
#if METHOD == StopWait
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"

int main(int argc, char* argv[])
{
	RdtSender *ps = new StopWaitRdtSender();
	RdtReceiver * pr = new StopWaitRdtReceiver();
//	pns->setRunMode(0);  //VERBOSģʽ
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
#endif

