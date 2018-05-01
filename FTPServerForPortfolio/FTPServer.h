#pragma once
#include "WinSockHeader.h"
#include "WaitClient.h"

#define SERVERPORT 210


class FTPServer {
public:
	~FTPServer() { WSACleanup(); };
	inline FTPServer() {if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);	}

	void Starter();

	void SetFilePath(string p) {
		setOrNot = true;
	};
private:
	WSADATA wsa;
	SOCKET listen_sock = INVALID_SOCKET;
	SOCKADDR_IN serveraddr;
	string path{ "" };
	WaitClients clients;
	bool setOrNot = false;

};

