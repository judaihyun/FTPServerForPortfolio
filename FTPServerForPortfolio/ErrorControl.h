#include <WinSock2.h>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

static unsigned long int LOG_NUMBER{ 0 };

enum logLevel {
	LOG_FETAL = 1, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG, LOG_TRACE
};



void err_quit(const char * msg);
void err_display(const char* msg);

void setLogLevel(logLevel level);
void ftpLog(logLevel, const char*, ...);
