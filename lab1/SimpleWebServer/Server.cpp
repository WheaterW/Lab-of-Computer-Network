//Coded by Wei Yunze
/*测试信息：
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

WSADATA wsaData;					//WSADATA，一种数据结构。这个结构被用来存储被WSAStartup函数调用后返回的Windows Sockets数据。它包含Winsock.dll执行的数据。
fd_set rfds;						//用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
fd_set wfds;						//用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
//ui state = unconnected;				//状态机预留信息
SOCKET srvSocket, sessionSocket;	//监听Socket和当前新建的会话Socket
sockaddr_in addr, clientAddr;		//本机地址和当前新建客户端地址。sockaddr_in此数据结构用做bind、connect、recvfrom、sendto等函数的参数，指明地址信息。但一般编程中并不直接针对此数据结构操作，而是使用另一个与sockaddr等价的数据结构
vector<CLIENTS> clients;				//当前的会话socket集合
vector<vector<CLIENTS>::iterator> todel;//亟待删除的集合
unsigned long ServerIP;				//配置IP使用的变量
int Port;							//配置IP使用的变量
string localpath;					//配置主目录
int addrLen;						//addr的长度
char recvBuf[4096];					//接受缓存区
time_t curtime;						//当前时间
u_long blockMode = 1;//将srvSock设为非阻塞模式以监听客户连接请求
bool server_stop = false;

/* 请求监听模块 */
int Listen() {
	while (true) {
		//描述符是一次性的。初始化清空read,write描述符,必须用FD_ZERO初始化描述符才能FD_SET
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		//设置监听socket的描述符
		FD_SET(srvSocket, &rfds);
		vector<CLIENTS>::iterator it = clients.begin();
		int rtn;
		//设置当前的sessionSockets
		for (; it != clients.end(); it++) {
			FD_SET(it->sessionSocket, &rfds);
			FD_SET(it->sessionSocket, &wfds);
		}
		//检测监听及当前的sessiongSockets是否有新动作
		//int positive = select(0, &rfds, &wfds, NULL, NULL);
		timeval tv;
		tv.tv_sec = 5; tv.tv_usec = 0;
		int positive = select(0, &rfds, NULL, NULL, &tv);		//问题：如何让浏览器判断发送数据结束
		cout << endl;

		if (positive <= 0) { if (errno == EINTR) continue; }
		//处理新进连接
		if (FD_ISSET(srvSocket, &rfds)) {
			printf("New connection found!\n");
			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);	//接受连接并产生新的socket
			if (sessionSocket != INVALID_SOCKET) {
				if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
					cout << "ioctlsocket() failed with error!\n";
					//return 0;
				}
				CLIENTS newclient(sessionSocket, clientAddr);
				time(&newclient.time);
				clients.push_back(newclient);				//将新建立的socket放入客户端集合中
				//让新加入的sessionSocket进入描述符中待命
				FD_SET(sessionSocket, &rfds);
				FD_SET(sessionSocket, &wfds);
			}
		}
		//遍历当前已连接节点
		it = clients.begin();
		//设置当前的sessiongSockets
		for (; it != clients.end(); it++) {
			SOCKET thissock = it->sessionSocket;
			//处理可读socket
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
					if (rtn == SOCKET_ERROR) {	//关闭异常Socket
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
			if (!it->KeepAlive || currTime - it->time > KEEP_ALIVE_TIME) {	// 关闭非持续连接和超时的持续连接
				if (find(todel.begin(), todel.end(), it) == todel.end()) {	// 只关闭先前没有关闭的连接
					closesocket(thissock);
					todel.push_back(it);
					if (it->KeepAlive) printf("Timeout! ");
					else printf("Not KeepAlive! ");
					printf("SessionSocket %d closed!\n", thissock);
				}
			}
		}

		//删除关闭了的sockets
		while (!todel.empty()) {
			clients.erase(*(todel.end()-1));
			todel.erase(todel.end() - 1);
		}	
		//输出当前剩余的sessionSockets
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
		getline(sline, part, '.');	//前8位
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

/* main----基础服务集 */
int main() {
	
	printf("Starting SimpleWebServer...\n");

	if (makeconfig() == EOF) {
		printf("Config File Error!\n");
		return 0;
	}
	
	//初始化
	int nRc = WSAStartup(0x0202, &wsaData);		//应用程序调用的第一个windockAPI函数，完成初始化。正常返回0；失败返回一些错误代码
	if (nRc) {
		printf("Winsock  startup failed with error!\n");
		return 0;
	}
	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
		return 0;
	}
	printf("Winsock startup Ok!\n");

	//创建一个socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);	//使用Internet地址，建立流类型的socket。参数表：地址族，socket类型和地址族使用的具体协议（0表示自动选择）
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");
	else {
		printf("Socket create Failed!\n");
		return 0;
	}

	//绑定到ip和端口号
	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	addr.sin_addr.S_un.S_addr = htonl(ServerIP);
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));	//将socket和主机地址绑定
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	else {
		printf("Socket bind Failed!\n");
		return 0;
	}

	//客户端地址信息
	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//监听，以等待客户机程序的连接请求
	rtn = listen(srvSocket, SOMAXCONN);	//参数分别为一个已经绑定但没有连接的socket和等待连接队列的长度
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");
	else {
		printf("Socket listen Failed!\n");
		return 0;
	}

	//设置非阻塞模式
	//ioctlsocket: This function controls the I/O mode of a socket，参数表中的&blockMode是配合FIONBIO使用的
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
		cout << "ioctlsocket() failed with error!\n";
		//return 0;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";


	printf("Listen Socket: %d\n", srvSocket);

	/* 进入监听模块循环监听 */
	//signal(SIGINT, sigint_handler);
	//SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
	Listen();
	printf("Server closed!\n");
	return 0;
}


//http://127.0.0.1:8080/index.html