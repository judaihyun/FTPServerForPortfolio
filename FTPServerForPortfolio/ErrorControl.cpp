#include "errorcontrol.h"


void err_quit(const char * msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

logLevel lev;

void setLogLevel(logLevel level) {
	lev = level;
}

void ftpLog(logLevel level, const char* format, ...) {
//#ifdef _DEBUG_MODE_
	char *buf = new char[512];
	
	va_list ap;
	lev = LOG_TRACE;

	if (lev >= level) {
		switch (level) {
		case LOG_TRACE: {   // 6
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "TRACE [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}
		case LOG_DEBUG: {   // 5
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "DEBUG [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}
		case LOG_INFO: {  // 4
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "INFO [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}
		case LOG_WARN: {  // 3
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "WARN [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}
		case LOG_ERROR: {  // 2
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "ERROR [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}
		case LOG_FETAL: {  // 1
			va_start(ap, format);
			vsprintf_s(buf, 512, format, ap);
			cout << setw(7) << "FETAL [" << setw(3) << LOG_NUMBER++ << setw(3) << "] " << buf << endl;
			break;
		}


		}
	}
	va_end(ap);
//#endif
}


