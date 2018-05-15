#pragma once

#include "WinSockHeader.h"
#include "ControlHandler.h"
#define SERVERPORT 210  //control channel

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class FtpServer {
public:

	~FtpServer() {
		ftpLog(LOG_DEBUG, "FtpServer() Destructed");
		closesocket(controlSock); WSACleanup();
	};
	FtpServer() {
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);
	};
	void Starter();
	void setPath(string r) {
		if (!r.empty()) {
			argList.rootPath = r;
			setOrNot = true;
		}
	};
	void setActivePort(int ap) { activePort = ap; }
	int getActivePort() { return activePort; }
	void setControlPort(int p) { controlPort = p; }
	int getControlPort() { return controlPort; }

	void accepting(SOCKET&);
private:
	WSADATA wsa;
	SOCKET listenSock = INVALID_SOCKET;
	SOCKET controlSock = INVALID_SOCKET;
	SOCKADDR_IN serverIp;
	SOCKADDR_IN clientaddr;
	SOCKADDR_IN controlAddr;

	int controlPort{ 210 };
	int activePort{ 200 };
	string serverIP{ "0.0.0.0" };

	bool setOrNot = false;
	HANDLE fThread1;
	passToThread argList;
};



