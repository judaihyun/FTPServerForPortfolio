#pragma once
#include "WinSockHeader.h"
#include "Utils.h"
#include <cctype>
#define BUFSIZE 512

struct passToThread {
	void* sock;
	string path;
};


//unsigned WINAPI DataChannel(void*);
unsigned WINAPI ControlChannel(void*);
void ResponseController(char *, char*, string);




class WaitClients {
public:

	~WaitClients() { closesocket(client_sock); };


	void Accepting(SOCKET&);
	void GetFiles();
	string GetPath() { return argList.path; };
	void setPath(string p) { argList.path = p; };
private:
	SOCKET client_sock = INVALID_SOCKET;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE fThread1; 
	HANDLE fThread2;

	passToThread argList;
	string path; // f:/DUMMY
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	
};


