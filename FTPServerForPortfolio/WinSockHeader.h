#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <filesystem>
#include <process.h>
#include <Ws2def.h>
#include <Wsrm.h>

#include "errorcontrol.h"
using namespace std;
namespace fs = std::experimental::filesystem;


struct passToThread {
	void* sock;
	int activePort{ 0 };
	string serverIP{ "" };
	string rootPath;
	int id{ 0 };
};