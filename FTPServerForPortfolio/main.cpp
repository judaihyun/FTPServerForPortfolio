#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")



#include "FtpServer.h"

void GetIPAddress(const char * ifr, unsigned char * out) {
}



int main() {

	/*
	clock_t start, end;
	start = clock();
	*/


	setLogLevel(logLevel::LOG_INFO);

	string path{ "" };
	int activePort{ 0 };
	int controlPort{ 0 };

	FtpServer s;
	
	cout << "(ex : c:/dir, d:, f:/dir/dir2)" << endl;
	cout << "FTP root path : ";
	cin >> path;
	

	s.setPath(path);   
	
	//s.setControlPort(controlPort);
	//s.setActivePort(activePort);

	cout << "-------------------setting--------------------\n";
	cout << "ActiveDataPort : " << s.getActivePort() << endl;
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