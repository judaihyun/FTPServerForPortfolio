#pragma once
#include "WinSockHeader.h"
#include "ControlHandler.h"
#include <cctype>
#define SERVERPORT 210  //control channel



class FtpServer {
public:

	~FtpServer() {
		ftpLog(LOG_DEBUG, "WaitClients Destructed");
		closesocket(controlSock); WSACleanup(); 
	};
	FtpServer(string r) {

		if (!r.empty()) {
			argList.rootPath = r;
			setOrNot = true;
		}
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);
	
	};
	void Starter();
	void accepting(SOCKET&);

private:
	WSADATA wsa;
	SOCKET listenSock = INVALID_SOCKET;
	SOCKET controlSock = INVALID_SOCKET;
	SOCKADDR_IN clientaddr;
	SOCKADDR_IN controlAddr;
	int addrlen{ 0 };


	bool setOrNot = false;
	HANDLE fThread1; 
	passToThread argList;
};

