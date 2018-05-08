#pragma once
#include "WinSockHeader.h"
#include <sstream>

void replaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter);

void replacePORT(std::string & source, std::string & ip, int & port);