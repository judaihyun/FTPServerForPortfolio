#include "Utils.h"




void ReplaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter)
{
	size_t iPos = strCallId.find(pszBefore);
	size_t iBeforeLen = strlen(pszBefore);

	while (iPos < std::string::npos)
	{
		strCallId.replace(iPos, iBeforeLen, pszAfter);
		iPos = strCallId.find(pszBefore, iPos);
	}
}

string GetPassivePort() {
	string port;
	int retval{ 0 };
	int len{ 0 };

	SOCKADDR_IN listenInfo;
	SOCKADDR_IN dataAddr;
	SOCKET dataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dataSock == INVALID_SOCKET) err_quit("data socket()");

	bool optval = true;
	retval = setsockopt(dataSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	ZeroMemory(&dataAddr, sizeof(dataAddr));
	dataAddr.sin_family = AF_INET;
	dataAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	dataAddr.sin_port = htons(0);

	retval = bind(dataSock, (SOCKADDR*)&dataAddr, sizeof(dataAddr));
	if (retval == SOCKET_ERROR) err_quit("data bind()");


	if (listen(dataSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}

	len = sizeof(listenInfo);
	getsockname(dataSock, (SOCKADDR*)&listenInfo, &len);

	printf("Client Socket Info :: %s (%d)\n", inet_ntoa(listenInfo.sin_addr), ntohs(listenInfo.sin_port));
	int sinPort = ntohs(listenInfo.sin_port);
	int highBit = (sinPort >> 8) & 255;
	int lowBit = sinPort & 255;

	port = "(";
	port += inet_ntoa(listenInfo.sin_addr);
	port += "," + to_string(highBit) + "," + to_string(lowBit) + ").";

	ReplaceString(port, ".", ",");



	return port;
}


