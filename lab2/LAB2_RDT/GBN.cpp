#include "stdafx.h"
#if METHOD == GBN

#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"

int main(int argc, char* argv[])
{
	RdtSender* ps = new GBNRdtSender();		//ʵ����GBNSender
	RdtReceiver* pr = new GBNRdtReceiver();	//ʵ����GBNReceiver
	pns->setRunMode(RunMode);  //����ģʽ����stdafx���޸�
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
#endif // METHOD == GBN
