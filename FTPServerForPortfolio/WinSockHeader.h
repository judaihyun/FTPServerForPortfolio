#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#define SERVERIP "192.168.219.101"
#define ACTIVEPORT 200

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <experimental\filesystem>
#include <filesystem>
#include <process.h>
#include <Ws2def.h>
#include <Wsrm.h>

#include "ErrorControl.h"
using namespace std;
namespace fs = std::experimental::filesystem;


struct passToThread {
	void* sock;
	string rootPath;
};


