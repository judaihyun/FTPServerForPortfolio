#define BUFSIZE 512
#include "WinSockHeader.h"
#include <cctype>

unsigned WINAPI DataChannel(void*);
unsigned WINAPI ControlChannel(void*);

HANDLE fThread1; 
HANDLE fThread2;

void Parser(char *, char*, string);

struct passToThread {
	void* sock;
	string path;
};

class WaitClients {
public:
	~WaitClients() { closesocket(client_sock); };
	void Accepting(SOCKET&);

	void GetFiles();

	string GetPath() { return argList.path; };
	void setPath(string p) { argList.path = p; }
private:
	SOCKET client_sock = INVALID_SOCKET;
	SOCKADDR_IN clientaddr;
	int addrlen;


	passToThread argList;
	string path; // f:/DUMMY
	queue<ifstream*> ifs;
	queue<ofstream*> ofs;
	queue<ifstream*> ifs2;
	queue<ofstream*> ofs2;
	vector<string> fileNames;
	
};


