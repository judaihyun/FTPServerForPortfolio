#pragma once
#include "WinSockHeader.h"

#define BUFSIZE 512
#define DIR_BUFSIZE 10000


unsigned WINAPI controlHandler(void*);

static int countForDebug{ 0 };

class ControlHandler {
public:
	ControlHandler(void* arg) :argList{ *((passToThread*)arg) } {
		controlSock = (SOCKET)argList.sock;
	}
	~ControlHandler() {
		closesocket(controlSock);
		closesocket(dataSock);
		closesocket(clientDataSock);
		cout << "ControlHandler ¼Ò¸êµÊ \n";
	}
	int getFileList();
	string createDataSock();
	string getRootPath() { if (rootPath == "") { err_quit("ControlHandler() root path"); } return rootPath; }
	string getCurPath() { if (curPath == "") { ftpLog("path not set", LOG_ERROR); }return curPath; }
	void addPath(string p) { curPath = p; }  // for CWD
	int controlActivate();
	int sendMsg(const string);
	int sendList();
	int commandsHandler();
private:
	passToThread argList;
	const string rootPath = argList.rootPath;
	string curPath{ "/" };
	string SP{ " " };
	string CRLF = "\r\n";

	char *dirList;
	//vector<string> dirList;
	char commands[40]{ '\0' };
	char resBuf[BUFSIZE + 1]{ "220 FTP Test Serivce. \r\n" };

	SOCKET controlSock = INVALID_SOCKET;
	SOCKET dataSock = INVALID_SOCKET;
	SOCKET clientDataSock = INVALID_SOCKET;

	SOCKADDR_IN controlClient_addr;
	SOCKADDR_IN dataClient_addr;
	SOCKADDR_IN dataServer_addr;
};
