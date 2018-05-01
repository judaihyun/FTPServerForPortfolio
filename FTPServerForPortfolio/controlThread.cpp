#include "WaitClient.h"
#include "Utils.h"
#include "ErrorControl.h"


int ControlHandler::controlActivate() {
	int addrlen{ 0 };
	int retval{ 0 };

	addrlen = sizeof(controlClient_addr);
	getpeername(controlSock, (SOCKADDR*)&controlClient_addr, &addrlen);
	printf("[controlChannel] by client : IP=%s, Port=%d\n", inet_ntoa(controlClient_addr.sin_addr), ntohs(controlClient_addr.sin_port));

	retval = send(controlSock, resBuf, strlen(resBuf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	while (1) {
		retval = recv(controlSock, commands, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}
		commands[retval] = '\0';


		if (2 == commandsHandler()) {
			cout << "@@@@";
		}

	
		
	}
	return 0;
}

int ControlHandler::sendMsg(const string msg) {
	int retval = send(controlSock, msg.c_str(), msg.length(), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	return retval;
}

int ControlHandler::commandsHandler() {

	cout << "commands : " << commands << endl;
	string path = "\"" + getPath() + "\"";
	int resLen = strlen(commands);

	char * context = NULL;
	char *arr[5];
	int cnt = 0;

	if (commands[resLen - 2] == '\r' && commands[resLen - 1] == '\n') {  // delete to carriage 
		commands[strlen(commands) - 2] = '\0';
	}

	char *token = strtok_s(commands, " ", &context);  
	while (token != NULL) {
		arr[cnt++] = token;
		token = strtok_s(NULL, " ", &context);
	}


	if (!strcmp(arr[0], "USER")) {
		sendMsg("230 logged in" + CRLF);
	}
	else if (!strcmp(arr[0], "PWD")) {
		sendMsg("257 " + path + " is current directory." + CRLF);
	}
	else if (!strcmp(arr[0], "TYPE")) {
		sendMsg("200 Type set to I(binary mode)" + CRLF);
	}
	else if (!strcmp(arr[0], "SYST")) {
		sendMsg("215 Windows_10" + CRLF);
	}
	else if (!strcmp(arr[0], "FEAT")) {
		sendMsg("211 END" + CRLF);
	}
	else if (!strcmp(arr[0], "PASV")) {
		sendMsg("227 Entering Passive Mode " + createDataSock() + CRLF);
	}
	else if (!strcmp(arr[0], "LIST")) {

		if (sendList() != 1) {
			ftpLog("LIST error", LOG_ERROR);
		}
		sendMsg("226 Successfully transferred " + path + CRLF);
	}
	else if (!strcmp(arr[0], "CWD")) {

	}

	return 1;
}



int ControlHandler::sendList() {

	int retval{ 0 };
	int addrlen{ 0 };
	//	cout << "dataChannel : " << dataSock << endl;

	addrlen = sizeof(dataClient_addr);

	clientDataSock = accept(dataSock, (SOCKADDR*)&dataClient_addr, &addrlen);
	if (clientDataSock == INVALID_SOCKET) {
		err_display("data accept()");
		return -1;
	}
	sendMsg("150 Openning data channel for directory listing of " + path + CRLF);

	printf("[dataChannel] by client : IP=%s, Port=%d\n", inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));

	string msg = "03-01-08  10:11PM       <DIR>          list.test" + CRLF;

	retval = send(clientDataSock, msg.c_str(), msg.length(), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	closesocket(clientDataSock);
	cout << "send size : " << retval << endl;

	return 0;

}



string ControlHandler::createDataSock() {
	
	string port;
	int retval{ 0 };
	int len{ 0 };

	dataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dataSock == INVALID_SOCKET) err_quit("data socket()");

	bool optval = true;
	retval = setsockopt(dataSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	ZeroMemory(&dataServer_addr, sizeof(dataServer_addr));
	dataServer_addr.sin_family = AF_INET;
	//dataServer_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	dataServer_addr.sin_addr.S_un.S_addr = inet_addr("192.168.219.101");
	dataServer_addr.sin_port = htons(0);

	retval = bind(dataSock, (SOCKADDR*)&dataServer_addr, sizeof(dataServer_addr));
	if (retval == SOCKET_ERROR) err_quit("data bind()");


	if (listen(dataSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}

	len = sizeof(dataServer_addr);
	getsockname(dataSock, (SOCKADDR*)&dataServer_addr, &len);
	printf("[dataChannel-server] : IP=%s, Port=%d\n", inet_ntoa(dataServer_addr.sin_addr), ntohs(dataServer_addr.sin_port));

	int sinPort = ntohs(dataServer_addr.sin_port);
	int highBit = (sinPort >> 8) & 255;
	int lowBit = sinPort & 255;

	port = "(";
	port += inet_ntoa(dataServer_addr.sin_addr);
	port += "," + to_string(highBit) + "," + to_string(lowBit) + " )";

	ReplaceString(port, ".", ",");

	port += ".";

	return port;
}
