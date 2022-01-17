#pragma once
#ifndef _SOURCES_H
#define _SOURCES_H

#include <iostream>
#include <map>
#include <WinSock2.h>
#include <ctime>

#include <WinSock2.h>
#include <stdio.h>
#include <set>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

class message;
bool response(istringstream& smsg, SOCKET s, string localpath);
message msgparser(istringstream& smsg);
int makeconfig(unsigned long& ServerIP, int& Port, string& localpath);

//socket��
struct CLIENTS {
	CLIENTS(SOCKET s, sockaddr_in a) : sessionSocket(s), addr(a), KeepAlive(false) {}
	SOCKET sessionSocket;
	sockaddr_in addr;
	time_t time;
	bool KeepAlive;
	//bool operator<(const CLIENTS& p) const {
	//	if (this->sessionSocket < p.sessionSocket) return true;
	//	else return false;
	//}
};

//������
struct message {
	//������
	string method;	//GET HEAD POST PUT
	string url;
	string rurl;	//remake url
	string version;
	//�ײ���
	string KeepAlive;	//keep-alive��close
};

struct fileinfo {
	fileinfo(string t, int c) : type(t), clength(c) {}
	string type;
	int clength;
};

#endif // !_SOURCES_H
