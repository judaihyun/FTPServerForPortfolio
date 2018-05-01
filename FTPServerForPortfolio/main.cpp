#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include "FTPServer.h"


int main(){

	clock_t start, end;
	start = clock();


	FTPServer s;
	s.SetFilePath("f:/DUMMY");
	s.Starter();

	/*
	WaitClients a;
	a.GetFiles("f:/DUMMY");
	*/




	end = clock();
	std::cout << "CLOCKS_PER_SEC " << CLOCKS_PER_SEC << "\n";
	std::cout << "CPU-TIME START " << start << "\n";
	std::cout << "CPU-TIME END " << end << "\n";
	std::cout << "CPU-TIME END - START " << end - start << "\n";
	std::cout << "TIME(SEC) " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "\n";
	std::cout << "TIME(MIN) " << (static_cast<double>(end - start) / CLOCKS_PER_SEC) / 60 << "\n";

	return 0;
}

