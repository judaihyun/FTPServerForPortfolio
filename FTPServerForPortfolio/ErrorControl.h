/*
#define LOG_FETAL 1
#define LOG_ERROR 2
#define LOG_WARN 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_TRACE 6
*/

#define LOG_LEVEL 4


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

void ftpLog(logLevel, const char*, ...);
