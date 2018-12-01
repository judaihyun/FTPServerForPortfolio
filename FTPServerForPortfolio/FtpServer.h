#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include "winsockheader.h"
#include "controlhandler.h"

#define SERVERPORT 210  //control channel


class FtpServer {
public:

	~FtpServer() {
		ftpLog(LOG_DEBUG, "FtpServer() Destructed");
		closesocket(controlSock); WSACleanup();
	};
	FtpServer() {
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);
	};
	void starter();
	void setPath(string rootPath);
	string getPath() { return argList.rootPath; }
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

	bool pathIsSet = false;
	HANDLE fThread1;
	passToThread argList;

};



