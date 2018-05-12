#pragma once
#include "WinSockHeader.h"


#define COM_BUFSIZE 1024  // for command
#define RES_BUFSIZE 1024  // for response
#define DIR_BUFSIZE 10000000
#define MAX_FILE_SIZE 10000000


unsigned WINAPI controlHandler(void*);

class ControlHandler {
public:
	ControlHandler(void* arg) : argList{ *((passToThread*)arg) } {
		controlSock = (SOCKET)argList.sock;
		activePort = argList.activePort;
		serverIP = argList.serverIP;
	}
	~ControlHandler() {
		ftpLog(LOG_INFO, "ControlHandler ¼Ò¸êµÊ");
		closesocket(controlSock);
		closesocket(dataListenSock);
		closesocket(clientDataSock);
	}

	int getFileList();
	void setFileName(string n) {fileName = n; 	};
	string getFileName() { return fileName; }
	int openFile();
	string createPASVSock(); // passive
	void createPORTSock(string); // active
	string getRootPath() { if (rootPath == "") { err_quit("ControlHandler() root path"); } return rootPath; }
	string getCurPath() { if (curPath == "") { ftpLog(LOG_ERROR,"path not set"); }return curPath; }
	void addPath(string p) { curPath += p; }  // for CWD
	void movePath(string p) { curPath = p; }  // for CWD
	int controlActivate();
	int sendMsg(const string);
	void sendFile();
	void setRetrSize(unsigned long int r) { sizeRETR = r; };
	unsigned long int getRetrSize() { return sizeRETR; };
	void sendList();
	void pasvSendList();
	void portSendList();
	int commandsHandler();
	void setActivePort(int ap) { activePort = ap; }
	int getActivePort() { return activePort; };
	void setServerIp(string ip) { serverIP = ip; }
	string getServerIp() { return serverIP; }
private:
	passToThread argList;  
	const string rootPath = argList.rootPath;  //set by main
	int activePort{ 0 };
	string serverIP{ "0.0.0.0" };
	string curPath{ "/" };  //current path
	string SP{ " " }; //space
	string CRLF{ "\r\n" };
	string fileName{ "" };
	unsigned long int sizeRETR{ 0 };
	bool isActive = false;
	char commands[COM_BUFSIZE]{ '\0' };
	char resBuf[RES_BUFSIZE + 1]{ "220 FTP Test Serivce. \r\n" };
	char *dirList;

	SOCKET controlSock = INVALID_SOCKET;
	SOCKET dataListenSock = INVALID_SOCKET; // for PASV
	SOCKET clientDataSock = INVALID_SOCKET; 



	SOCKADDR_IN controlClient_addr;
	SOCKADDR_IN dataListen_addr; // for PASV
	SOCKADDR_IN dataClient_addr; // for PASV
	SOCKADDR_IN dataCon_addr;    // for Active mode

	ifstream *ifs;
	/*
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	*/
};
