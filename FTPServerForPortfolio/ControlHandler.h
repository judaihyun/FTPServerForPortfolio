#pragma once
#include "WinSockHeader.h"

#define COM_BUFSIZE 1024
#define SEND_BUFSIZE 1460
#define DIR_BUFSIZE 10000


unsigned WINAPI controlHandler(void*);

static int countForDebug{ 0 };

class ControlHandler {
public:
	ControlHandler(void* arg) :argList{ *((passToThread*)arg) } {
		controlSock = (SOCKET)argList.sock;
	}
	~ControlHandler() {
		cout << "ControlHandler ¼Ò¸êµÊ \n";
		closesocket(controlSock);
		closesocket(dataSock);
		closesocket(clientDataSock);
	}

	int getFileList();
	void setFileName(string n) {fileName = n; 	};
	string getFileName() { return fileName; }
	int openFile();
	string createDataSock();
	string getRootPath() { if (rootPath == "") { err_quit("ControlHandler() root path"); } return rootPath; }
	string getCurPath() { if (curPath == "") { ftpLog(LOG_ERROR,"path not set"); }return curPath; }
	void addPath(string p) { curPath += p; }  // for CWD
	void movePath(string p) { curPath = p; }  // for CWD
	int controlActivate();
	int sendMsg(const string);
	void sendFile();
	void sendList();
	int commandsHandler();
private:
	passToThread argList;  
	const string rootPath = argList.rootPath;  //set by main
	string curPath{ "/" };  //current path
	string SP{ " " }; //space
	string CRLF{ "\r\n" };
	string fileName{ "" };

	char *dirList;
	char commands[40]{ '\0' };
	char resBuf[COM_BUFSIZE + 1]{ "220 FTP Test Serivce. \r\n" };

	SOCKET controlSock = INVALID_SOCKET;
	SOCKET dataSock = INVALID_SOCKET;
	SOCKET clientDataSock = INVALID_SOCKET;

	SOCKADDR_IN controlClient_addr;
	SOCKADDR_IN dataClient_addr;
	SOCKADDR_IN dataServer_addr;

	ifstream *ifs;
	/*
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	*/
};
