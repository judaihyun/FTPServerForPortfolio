#pragma once
#include "WinSockHeader.h"
#include <cctype>
#define BUFSIZE 512

struct passToThread {
	void* sock;
	string path;
};

unsigned WINAPI controlHandler(void*);

class WaitClients {
public:

	~WaitClients() { closesocket(controlSock); };
	void accepting(SOCKET&);
	//void dataAccept();
	void getFiles();
	string getPath() { return path; };
	void setPath(string p) { argList.path = p; };
private:
	SOCKET controlSock = INVALID_SOCKET;
	SOCKADDR_IN clientaddr;
	int addrlen{ 0 };

	HANDLE fThread1; 
	passToThread argList;

	string path; // f:/DUMMY
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	
};



class ControlHandler {
public:
	ControlHandler(void* arg) :argList{ *((passToThread*)arg) } {
		path = argList.path;
		controlSock = (SOCKET)argList.sock;
	}
	~ControlHandler() {
		closesocket(controlSock);
		closesocket(dataSock);
		closesocket(clientDataSock);
	}

	string createDataSock();
	string getPath() { return path; }
	int controlActivate();
	int sendMsg(const string);
	int sendList();
	int commandsHandler();
private:
	passToThread argList;
	string path{ "" };
	string CRLF = "\r\n";

	char commands[40]{ '\0', };
	char resBuf[BUFSIZE + 1]{ "220 FTP Test Serivce. \r\n" };

	SOCKET controlSock = INVALID_SOCKET;
	SOCKET dataSock = INVALID_SOCKET;
	SOCKET clientDataSock = INVALID_SOCKET;

	SOCKADDR_IN controlClient_addr;
	SOCKADDR_IN dataClient_addr;
	SOCKADDR_IN dataServer_addr;
};
