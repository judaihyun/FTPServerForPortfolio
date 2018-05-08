#include "Utils.h"




void replaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter)
{
	size_t iPos = strCallId.find(pszBefore);
	size_t iBeforeLen = strlen(pszBefore);

	while (iPos < std::string::npos)
	{
		strCallId.replace(iPos, iBeforeLen, pszAfter);
		iPos = strCallId.find(pszBefore, iPos);
	}
}

void replacePORT(std::string & source, std::string & ip, int & port) {
	replaceString(source, ",", ".");
	string temp;
	int high = 0;
	int low = 0;
	int pos = 0;
	int i;
	for (i = 0; pos < 4; ++i) {
		if (source[i] == '.') {
			pos++;
		}
	}
	ip.assign(source, 0, i - 1);
	temp.assign(source, i, source.size());
   
	low = stoi(temp.substr(temp.find(".") + 1, temp.size()));
	high = stoi(temp.substr(0, temp.find(".")));

	port = (high * 256) + low;

}

void printPerms(fs::perms p)
{
	std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-");
}   // usage : 				printPerms(fs::status(p).permissions());