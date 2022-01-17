//Coded by Wei Yunze
/*������Ϣ��
http://127.0.0.1:8080
http://127.0.0.1:8080/index.html
http://127.0.0.1:8080/Delacey - Dream It Possible.mp3
http://127.0.0.1:8080/huawei_logo.webp
http://127.0.0.1:8080/id-pop.webm
*/
#pragma once
#include "sources.h"

#define KEEP_ALIVE_TIME 10
#define configpath "config/IP.config"
typedef unsigned int ui;
using namespace std;

WSADATA wsaData;					//WSADATA��һ�����ݽṹ������ṹ�������洢��WSAStartup�������ú󷵻ص�Windows Sockets���ݡ�������Winsock.dllִ�е����ݡ�
fd_set rfds;						//���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�������ݵ�����
fd_set wfds;						//���ڼ��socket�Ƿ���Է��͵��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�����Է������ݣ�
//ui state = unconnected;				//״̬��Ԥ����Ϣ
SOCKET srvSocket, sessionSocket;	//����Socket�͵�ǰ�½��ĻỰSocket
sockaddr_in addr, clientAddr;		//������ַ�͵�ǰ�½��ͻ��˵�ַ��sockaddr_in�����ݽṹ����bind��connect��recvfrom��sendto�Ⱥ����Ĳ�����ָ����ַ��Ϣ����һ�����в���ֱ����Դ����ݽṹ����������ʹ����һ����sockaddr�ȼ۵����ݽṹ
vector<CLIENTS> clients;				//��ǰ�ĻỰsocket����
vector<vector<CLIENTS>::iterator> todel;//ؽ��ɾ���ļ���
unsigned long ServerIP;				//����IPʹ�õı���
int Port;							//����IPʹ�õı���
string localpath;					//������Ŀ¼
int addrLen;						//addr�ĳ���
char recvBuf[4096];					//���ܻ�����
time_t curtime;						//��ǰʱ��
u_long blockMode = 1;//��srvSock��Ϊ������ģʽ�Լ����ͻ���������
bool server_stop = false;

/* �������ģ�� */
int Listen() {
	while (true) {
		//��������һ���Եġ���ʼ�����read,write������,������FD_ZERO��ʼ������������FD_SET
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		//���ü���socket��������
		FD_SET(srvSocket, &rfds);
		vector<CLIENTS>::iterator it = clients.begin();
		int rtn;
		//���õ�ǰ��sessionSockets
		for (; it != clients.end(); it++) {
			FD_SET(it->sessionSocket, &rfds);
			FD_SET(it->sessionSocket, &wfds);
		}
		//����������ǰ��sessiongSockets�Ƿ����¶���
		//int positive = select(0, &rfds, &wfds, NULL, NULL);
		timeval tv;
		tv.tv_sec = 5; tv.tv_usec = 0;
		int positive = select(0, &rfds, NULL, NULL, &tv);		//���⣺�����������жϷ������ݽ���
		cout << endl;

		if (positive <= 0) { if (errno == EINTR) continue; }
		//�����½�����
		if (FD_ISSET(srvSocket, &rfds)) {
			printf("New connection found!\n");
			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);	//�������Ӳ������µ�socket
			if (sessionSocket != INVALID_SOCKET) {
				if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
					cout << "ioctlsocket() failed with error!\n";
					//return 0;
				}
				CLIENTS newclient(sessionSocket, clientAddr);
				time(&newclient.time);
				clients.push_back(newclient);				//���½�����socket����ͻ��˼�����
				//���¼����sessionSocket�����������д���
				FD_SET(sessionSocket, &rfds);
				FD_SET(sessionSocket, &wfds);
			}
		}
		//������ǰ�����ӽڵ�
		it = clients.begin();
		//���õ�ǰ��sessiongSockets
		for (; it != clients.end(); it++) {
			SOCKET thissock = it->sessionSocket;
			//����ɶ�socket
			if (FD_ISSET(thissock, &rfds)) {
				printf("Socket %d is readable!\n", thissock);
				memset(recvBuf, '\0', 4096);
				rtn = recv(thissock, recvBuf, 4096, 0);
				//if (rtn == SOCKET_ERROR) printf("SessionSocket %d read error!\n", thissock);
				if (rtn > 0) {
					string clientinfo = inet_ntoa(it->addr.sin_addr);
					printf("\nReceived %d bytes of message from %s:%d: \n%s\n", rtn, clientinfo.c_str(), it->addr.sin_port, recvBuf);
					string msg(recvBuf);
					istringstream smsg(msg);
					it->KeepAlive = response(smsg, thissock, localpath);
				}
				else {
					if (rtn == SOCKET_ERROR) {	//�ر��쳣Socket
						printf("SessionSocket %d read error!\n", thissock);
						closesocket(thissock);
						todel.push_back(it);
						printf("SessionSocket %d closed!\n", thissock);
					}
					else printf("Read Nothing\n");
				}
			}
			time_t currTime;
			time(&currTime);
			if (!it->KeepAlive || currTime - it->time > KEEP_ALIVE_TIME) {	// �رշǳ������Ӻͳ�ʱ�ĳ�������
				if (find(todel.begin(), todel.end(), it) == todel.end()) {	// ֻ�ر���ǰû�йرյ�����
					closesocket(thissock);
					todel.push_back(it);
					if (it->KeepAlive) printf("Timeout! ");
					else printf("Not KeepAlive! ");
					printf("SessionSocket %d closed!\n", thissock);
				}
			}
		}

		//ɾ���ر��˵�sockets
		while (!todel.empty()) {
			clients.erase(*(todel.end()-1));
			todel.erase(todel.end() - 1);
		}	
		//�����ǰʣ���sessionSockets
		printf("\nCurrent sessionSockets: ");
		//it = clients.begin();
		//for (; it != clients.end(); it++) { printf("%d ", it->sessionSocket); }
		//printf("\n
		it = clients.begin();
		for (; it != clients.end(); it++) {
			printf("%d ", it->sessionSocket);
		}
		printf("\n");
		
		if (server_stop) {
			it = clients.begin();
			for (; it != clients.end(); it++) {
				closesocket(it->sessionSocket);
				printf("Socket %d closed!\n", it->sessionSocket);
			}
			break;
		}
	}
	return 0;
}


int makeconfig() {
	fstream fs(configpath);
	if (!fs.is_open()) {
		cout << "Config file not found!" << endl;
		return -1;
	}
	else {
		string line, IP, part, port;
		//int tmp;
		getline(fs, line);
		IP = line.substr(line.find(":") + 2, string::npos);
		istringstream sline(IP);
		ServerIP = 0;
		getline(sline, part, '.');	//ǰ8λ
		ServerIP += stoi(part) << 24;
		///cout << ServerIP << endl;
		getline(sline, part, '.');
		ServerIP += stoi(part) << 16;
		//cout << ServerIP << endl;
		getline(sline, part, '.');
		ServerIP += stoi(part) << 8;
		//cout << ServerIP << endl;
		getline(sline, part);
		ServerIP += stoi(part);
		//cout << ServerIP << endl;
		getline(fs, line);
		port = line.substr(line.find(":") + 2, string::npos);
		Port = stoi(port);
		getline(fs, localpath);
		localpath = localpath.substr(localpath.find(":") + 2, string::npos);
	}
	fs.close();
	return 0;
}

//void sigint_handler(int sig) {
//	if (sig == SIGINT) {
//		cout << "Server stopping!" << endl;
//		server_stop = true;
//	}
//}

static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT: // Ctrl+C
		server_stop = true;
		return true;
		break;
	case CTRL_BREAK_EVENT: // Ctrl+Break
		break;
	case CTRL_CLOSE_EVENT: // Closing the console window
		break;
	case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
		break;
	case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
		break;
	}

	// Return TRUE if handled this message, further handler functions won't be called.
	// Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
	return FALSE;
}

/* main----�������� */
int main() {
	
	printf("Starting SimpleWebServer...\n");

	if (makeconfig() == EOF) {
		printf("Config File Error!\n");
		return 0;
	}
	
	//��ʼ��
	int nRc = WSAStartup(0x0202, &wsaData);		//Ӧ�ó�����õĵ�һ��windockAPI��������ɳ�ʼ������������0��ʧ�ܷ���һЩ�������
	if (nRc) {
		printf("Winsock  startup failed with error!\n");
		return 0;
	}
	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
		return 0;
	}
	printf("Winsock startup Ok!\n");

	//����һ��socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);	//ʹ��Internet��ַ�����������͵�socket����������ַ�壬socket���ͺ͵�ַ��ʹ�õľ���Э�飨0��ʾ�Զ�ѡ��
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");
	else {
		printf("Socket create Failed!\n");
		return 0;
	}

	//�󶨵�ip�Ͷ˿ں�
	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	addr.sin_addr.S_un.S_addr = htonl(ServerIP);
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));	//��socket��������ַ��
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	else {
		printf("Socket bind Failed!\n");
		return 0;
	}

	//�ͻ��˵�ַ��Ϣ
	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//�������Եȴ��ͻ����������������
	rtn = listen(srvSocket, SOMAXCONN);	//�����ֱ�Ϊһ���Ѿ��󶨵�û�����ӵ�socket�͵ȴ����Ӷ��еĳ���
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");
	else {
		printf("Socket listen Failed!\n");
		return 0;
	}

	//���÷�����ģʽ
	//ioctlsocket: This function controls the I/O mode of a socket���������е�&blockMode�����FIONBIOʹ�õ�
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
		cout << "ioctlsocket() failed with error!\n";
		//return 0;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";


	printf("Listen Socket: %d\n", srvSocket);

	/* �������ģ��ѭ������ */
	//signal(SIGINT, sigint_handler);
	//SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
	Listen();
	printf("Server closed!\n");
	return 0;
}


//http://127.0.0.1:8080/index.html