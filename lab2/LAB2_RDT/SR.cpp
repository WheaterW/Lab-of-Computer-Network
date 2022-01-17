#include "stdafx.h"
#if METHOD == SR

#include "SRRdtReceiver.h"
#include "SRRdtSender.h"

int main() {
	RdtSender* ps = new SRRdtSender();		//ʵ����GBNSender
	RdtReceiver* pr = new SRRdtReceiver();	//ʵ����GBNReceiver
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
#endif // METHOD == SR
