#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include "FtpServer.h"

int main() {

	/*
	clock_t start, end;
	start = clock();
	*/


	setLogLevel(logLevel::LOG_INFO);

	string ip{ "" };
	string path{ "" };
	int activePort{ 0 };
	int controlPort{ 0 };

	/*
	cout << "ACTIVE Data Port : ";
	cin >> activePort;
	*/


	
	cout << "ip : ";
	cin >> ip;
	cout << "Control Port : ";
	cin >> controlPort;
	cout << "FTP root path : " << "(ex : c:/dir, d:, f:/dir/dir2)" << endl;
	cin >> path;

	FtpServer s;  
	s.setPath(path);   
	s.setIp(ip);
	s.setControlPort(controlPort);
	//s.setActivePort(activePort);

	cout << "-------------------setting--------------------\n";
	cout << ">> ip : " << s.getIp() << ", ActiveDataPort : " << s.getActivePort() << endl;
	cout << ">> controlPort : " << s.getControlPort() << " , path : " << path << endl;
	cout << "-------------------setting--------------------\n";


	s.Starter();


	/*
	end = clock();
	std::cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
	std::cout << "CPU-TIME START " << start << "\n";
	std::cout << "CPU-TIME END " << end << "\n";
	std::cout << "CPU-TIME END - START " << end - start << "\n";
	std::cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";
	std::cout << "TIME(MIN) " << (static_cast<double>(end - start) / CLOCKS_PER_SEC) / 60 << "\n";
	*/

	return 0;
}