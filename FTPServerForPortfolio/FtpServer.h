#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include "WinSockHeader.h"
#include "ControlHandler.h"

#define LIBSSH2_INIT_NO_CRYPTO 0x0001
#define SERVERPORT 210  //control channel

/*
const char *keyfile1 = ".ssh/id_rsa.pub";
const char *keyfile2 = ".ssh/id_rsa";
const char *username = "username";
const char *password = "password";
const char *sftppath = "f:";
*/


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

	void setSecureFTP(bool i) { secureFTP = i; }

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

	bool secureFTP = NULL;
	
	int sock, i, auth_pw = 0;
	const char *fingerprint;
	char *userauthlist;
	LIBSSH2_SESSION *session;
	int rc;
	LIBSSH2_SFTP *sftp_session;
	LIBSSH2_SFTP_HANDLE *sftp_handle;
	
};



