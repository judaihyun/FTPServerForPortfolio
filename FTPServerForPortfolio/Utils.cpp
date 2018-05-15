#include "Utils.h"



void commandSeparator(std::vector<string> & argv, char *commands) {
	char * context = nullptr;
	std::string str{ "" };

	int resLen = strlen(commands);

	if (commands[resLen - 2] == '\r' && commands[resLen - 1] == '\n') {  // delete to cargviage 
		commands[resLen - 2] = '\0';
	}
	char *token = strtok_s(commands, " ", &context);

	str = commands;
	std::transform(str.begin(), str.end(), str.begin(), toupper);
	argv.push_back(str);
	argv.push_back(context);

	ftpLog(LOG_TRACE, "commands : %s, argc : %d, argv : %s", commands, argv.size(), context);

}



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

void portDecoder(std::string & source, std::string & ip, int & port, std::string serverIp) { //using active mode
	replaceString(source, ",", ".");
	string temp;
	int high = 0;
	int low = 0;
	int pos = 0;
	int i = 0;
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

std::string portEncoder(int sinPort, string addr) {       // using passive mode
														  //cout << "addr[" << addr << "]\n";
	string port{ "(" };

	int highBit = (sinPort >> 8) & 255;
	int lowBit = sinPort & 255;

	port += addr;
	port += "," + to_string(highBit) + "," + to_string(lowBit) + ")";

	replaceString(port, ".", ",");
	port += ".";

	return port;
}


void printStatus(const fs::path& p, fs::file_status s)
{
	printPerms(fs::status(p).permissions());
	cout << " " << p;
	// alternative: switch(s.type()) { case fs::file_type::regular: ...}
	if (fs::is_regular_file(s)) std::cout << " is a regular file\n";
	if (fs::is_directory(s)) std::cout << " is a directory\n";
	if (fs::is_block_file(s)) std::cout << " is a block device\n";
	if (fs::is_character_file(s)) std::cout << " is a character device\n";
	if (fs::is_fifo(s)) std::cout << " is a named IPC pipe\n";
	if (fs::is_socket(s)) std::cout << " is a named IPC socket\n";
	if (fs::is_symlink(s)) std::cout << " is a symlink\n";
	if (!fs::exists(s)) std::cout << " does not exist\n";
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