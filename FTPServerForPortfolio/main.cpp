#include "ftpserver.h"


int main() {
	SetConsoleTitle("SERVER");

	setLogLevel(logLevel::LOG_INFO);

	FtpServer s;
	s.setPath("f:");


	cout << "-------------------setting--------------------\n";
	cout << "ActiveDataPort : " << s.getActivePort() << endl;
	cout << ">> controlPort : " << s.getControlPort() << " , path : " << s.getPath() << endl;
	cout << "-------------------setting--------------------\n";


	s.starter();



	return 0;
}