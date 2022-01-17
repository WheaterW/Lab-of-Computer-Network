#pragma once
#include "winsock2.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include "sources.h"

#define port 8080
#define unconnected 0
#define connected 1

#define httpversion "HTTP/1.1 "
#define f404 "/404.html"
#define f400 "/400.html"

typedef unsigned int ui;

using namespace std;

#pragma comment(lib,"ws2_32.lib")

static enum MSGs {
	NOTDEF, GET, POST, HEAD, PUT
};
static map<string, MSGs> types;

static enum ITMS {
	NOTDEF1, Host, Connection
};
static map<string, ITMS> items;

static enum FTYPES {
	NOTDEF2, html, webm, mp4, mp3, webp, jpg, jpeg, png
};
static map<string, FTYPES> ftypes;

void Initialize() {
	types["GET"] = GET;
	types["POST"] = POST;
	types["HEAD"] = HEAD;
	types["PUT"] = PUT;
	items["Connection"] = Connection;
	items["Host"] = Host;
	ftypes["html"] = html;
	ftypes["webp"] = webp;
	ftypes["mp4"] = mp4;
	ftypes["mp3"] = mp3;
	ftypes["webm"] = webm;
	ftypes["jpg"] = jpg;
	ftypes["jpeg"] = jpeg;
	ftypes["png"] = png;
}

/* 报文解析 */
message msgparser(istringstream& smsg) {
	message res;
	string line;
	string word;
	getline(smsg, line);
	istringstream sline(line);
	getline(sline, res.method, ' ');
	getline(sline, res.url, ' ');
	getline(sline, res.version, ' ');
	while (getline(smsg, line)) {
		istringstream sline(line);
		getline(sline, word, ':');
		switch (items[word])
		{
		case Connection:
			getline(sline, res.KeepAlive);
			res.KeepAlive.erase(res.KeepAlive.begin());
			break;
		default:
			break;
		}
	}
	//去除url中的空格
	size_t pos;	//%20的位置
	while ((pos = res.url.find("%20")) != string::npos) {
		res.url.replace(pos, 3, " ");
	}
	return res;
}

/* 响应报文生成 */
//返回内容的长度
fileinfo getFile(string& res, string filename) {
	string judge = filename.substr(filename.find_last_of('.') + 1);
	auto ss = ostringstream{};
	if (judge == "html") { fstream fs(filename, ios::in); ss << fs.rdbuf(); }
	else { fstream fs(filename, ios::in | ios::binary); ss << fs.rdbuf(); }
	res = res + ss.str();
	return fileinfo(judge, ss.str().length());
}
//添加报文头
int makehead(string& res, int state_code, fileinfo info, message& msg) {
	time_t currtime; time(&currtime);	//获取当前时间（s）
	res = "\r\n" + res;
	res = "content-length: " + to_string(info.clength) + "\r\n" + res;
	switch (ftypes[info.type])
	{
	case html: res = "content-type: text/html\r\n" + res; break;
	case webp: res = "content-type: image/webp\r\n" + res; break;
	case mp4: res = "content-type: video/mp4\r\n" + res; break;
	case mp3: res = "content-type: audio/mp3\r\n" + res; break;
	case webm: res = "content-type: video/webm\r\n" + res; break;
	case jpg: res = "content-type: image/jpeg\r\n" + res;  break;
	case jpeg: res = "content-type: image/jpeg\r\n" + res;  break;
	case png: res = "content-type: image/png\r\n" + res; break;
	default: break;
	}
	res = "server: WYZ's SimpleWebServer/1.0.0 (Windows)\r\n" + res;
	res = "date: " + string(ctime(&currtime)) + res;		//string隐藏了\r\n
	res = "connection: " + msg.KeepAlive + "\r\n" + res;
	switch (state_code)
	{
	case 200: res = httpversion + to_string(state_code) + " OK\r\n" + res; break;
	case 400: res = httpversion + to_string(state_code) + " BadRequest\r\n" + res; break;
	case 404: res = httpversion + to_string(state_code) + " NotFound\r\n" + res; break;
	default: break;
	}
	//cout << res << endl;
	return 0;
}

/* 报文发送 */
int OK(SOCKET s, string rurl, message& msg, string filename) {
	string res;
	fileinfo info = getFile(res, filename);
	makehead(res, 200, info, msg);
	cout << "Reply:" << endl << res << endl;
	send(s, res.c_str(), res.length(), 0);
	return 0;
}

int BadRequest(SOCKET s, message& msg, string localpath) {
	string res;
	fileinfo info = getFile(res, string(localpath) + f400);
	makehead(res, 400, info, msg);
	cout << "Reply:" << endl << res << endl;
	send(s, res.c_str(), res.length(), 0);
	return 0;
}

int NotFound(SOCKET s, message& msg, string localpath) {
	string res;
	fileinfo info = getFile(res, string(localpath) + f404);
	makehead(res, 404, info, msg);
	cout << "Reply:" << endl << res << endl;
	send(s, res.c_str(), res.length(), 0);
	return 0;
}

/* 请求响应模块——报文类型判断 */
bool response(istringstream& smsg, SOCKET s, string localpath) {
	Initialize();
	message msg = msgparser(smsg);
	msg.rurl = localpath + msg.url;
	fstream fs(msg.rurl);
	if (!fs.is_open()) {
		cout << "File not found!" << endl;
		NotFound(s, msg, localpath);
		return 0;
	}
	else { fs.close(); }
	switch (types[msg.method])
	{
	case GET:
		OK(s, msg.rurl, msg, msg.rurl);
		break;
	case POST:
		break;
	case PUT:
		break;
	case HEAD:
		break;
	default:
		BadRequest(s, msg, localpath);
		break;
	}
	fs.close();
	printf("Socket %d responsed! Keep_Alive: %d %s\n", s, msg.KeepAlive[0] == 'k', msg.KeepAlive.c_str());
	return msg.KeepAlive[0] == 'k';
}