#pragma once
#include "WinSockHeader.h"
#include "ControlHandler.h"
#include <cctype>
#define SERVERPORT 210


class WaitClients {
public:

	~WaitClients() { closesocket(controlSock); WSACleanup(); };
	WaitClients(string r) {
		argList.rootPath = r;
		if (!r.empty()) {
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

	string rootPath{ "" }; // f:/DUMMY
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	
};

