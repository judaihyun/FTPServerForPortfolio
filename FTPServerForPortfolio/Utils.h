#pragma once
#include "WinSockHeader.h"
#include <sstream>

void replaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter);

void portDecoder(std::string &, std::string& , int &, std::string);

std::string portEncoder(int, string);

void commandSeparator(std::vector<string> &, char *);

void printPerms(fs::perms p);

void printStatus(const fs::path& p, fs::file_status s);