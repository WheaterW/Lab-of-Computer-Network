// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
#pragma once
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
using namespace std;
#define inputpath "C:\\Users\\WYZ\\Desktop\\一些实验\\计算机网络\\实验二\\Windows VS2019\\input.txt"
#define outputpath "C:\\Users\\WYZ\\Desktop\\一些实验\\计算机网络\\实验二\\Windows VS2019\\output.txt"
#define libpath "C:\\Users\\WYZ\\Desktop\\一些实验\\计算机网络\\实验二\\Windows VS2019\\netsimlib.lib"
#define GBN 0
#define SR 1
#define TCP 2
// TODO: 在此处引用程序需要的其他头文件
#pragma comment (lib, libpath)

#include "DataStructure.h"
#include "Global.h"
#include "NetworkService.h"
#include "RandomEventEnum.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "Tool.h"

#define METHOD TCP



//#pragma warning(disable:4482)
