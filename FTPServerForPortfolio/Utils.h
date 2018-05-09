#pragma once
#include "WinSockHeader.h"
#include <sstream>

void replaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter);

void portDecoder(std::string &, std::string& , int &, const char*);

string portEncoder(int, string);

void commandSeparator(std::vector<string> &, char *);