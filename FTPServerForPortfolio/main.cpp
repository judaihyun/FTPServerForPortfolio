#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")


#include "FtpServer.h"


int main() {
	SetConsoleTitle("SERVER");

	setLogLevel(logLevel::LOG_INFO);

	string path{ "" };
	int activePort{ 0 };
	int controlPort{ 0 };

	FtpServer s;
	s.setSecureFTP(true);
	

	s.setPath("f:");   
	

	cout << "-------------------setting--------------------\n";
	cout << "ActiveDataPort : " << s.getActivePort() << endl;
	cout << ">> controlPort : " << s.getControlPort() << " , path : " << path << endl;
	cout << "-------------------setting--------------------\n";


	s.Starter();



	return 0;
}