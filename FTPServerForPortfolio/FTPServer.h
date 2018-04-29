#pragma once
#include "WinSockHeader.h"
#include "WaitClient.h"

#define SERVERPORT 210


class FTPServer {
public:
	~FTPServer() { WSACleanup(); };
	FTPServer();
	void Start();
	
	void SetUploadFilePath(string p) {
		clients.setPath(p);
		setOrNot = true;
	};
	
private:
	WSADATA wsa;
	SOCKET listen_sock = INVALID_SOCKET;
	SOCKADDR_IN serveraddr;
	string path{ "" };
	WaitClients clients;
	WaitClients dataChannel;
	bool setOrNot = false;

};

