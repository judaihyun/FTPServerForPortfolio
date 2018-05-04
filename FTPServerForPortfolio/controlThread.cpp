#include "ControlHandler.h"
#include "Utils.h"

unsigned WINAPI controlHandler(void *arg)
{
	ControlHandler t = new ControlHandler(arg);
	t.controlActivate();
	return 0;
}

int ControlHandler::sendMsg(const string msg) {
	int retval = send(controlSock, msg.c_str(), msg.length(), 0);
	if (retval == SOCKET_ERROR) {
		err_display("sendMsg()");
	}

	return retval;
}


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



int ControlHandler::commandsHandler() {

	int resLen = strlen(commands);

	char * context = NULL;
	vector<char*> argv;
	
	int cnt = 0;

	if (commands[resLen - 2] == '\r' && commands[resLen - 1] == '\n') {  // delete to cargviage 
		commands[strlen(commands) - 2] = '\0';
	}

	char *token = strtok_s(commands, " ", &context);  
	while (token != NULL) {
		argv.push_back(token);
		token = strtok_s(NULL, " ", &context);
	}

	cout << "[" << countForDebug << "]commands : " << commands << ", argc : " << argv.size() << endl;
	cout << "current Path : " << curPath << endl;


	if (!strcmp(argv[0], "USER")) {
		sendMsg("230 logged in" + CRLF);
	}
	else if (!strcmp(argv[0], "PWD")) {
		sendMsg("257 \"" + getCurPath() + "\" is current directory." + CRLF);
	}
	else if (!strcmp(argv[0], "TYPE")) {
		//override Type
		sendMsg("200 Type set to I(binary mode)" + CRLF);
	}
	else if (!strcmp(argv[0], "SYST")) {
		sendMsg("215 Windows_10" + CRLF);
	}
	else if (!strcmp(argv[0], "FEAT")) {
		sendMsg("211 END" + CRLF);
	}
	else if (!strcmp(argv[0], "PASV")) {
		sendMsg("227 Entering Passive Mode " + createDataSock() + CRLF);
	}
	else if (!strcmp(argv[0], "LIST")) {
		cout << "LIST  curPath : " << getCurPath() << endl;
		if (getCurPath()=="") {
			cout << "emtpy" << endl;
			return 2;
		}
		if (sendList() >= 1) {
			ftpLog("LIST error", LOG_ERROR);
		}
		sendMsg("226 Successfully transferred " + getCurPath() + "\"" + CRLF);
	}
	else if (!strcmp(argv[0], "CWD")) {
		string temp{ "" };
		cout << "CWD argc : " << argv.size() << endl;
		cout << "CWD [1] " << argv[1] << endl;
		if (argv[1][0] == '/') {
			cout << " found / " << endl;
		}
		for (unsigned int i = 1; i < argv.size(); ++i){
			temp += argv[i];
			temp += " ";
		}
		temp.erase(temp.size()-1);

		addPath(temp);

		sendMsg("250 CWD command successful." + CRLF);

	}
	/*
	else if (!strcmp(argv[0], "RETR")) {   // file(s) are transffered from server to client.  RETR <FILENAME>

	}
	*/
	countForDebug++;
	cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;

	return 1;
}



int ControlHandler::sendList() {

	int retval{ 0 };
	int addrlen{ 0 };

	addrlen = sizeof(dataClient_addr);

	clientDataSock = accept(dataSock, (SOCKADDR*)&dataClient_addr, &addrlen);
	if (clientDataSock == INVALID_SOCKET) {
		err_display("data accept()");
		return -1;
	}
	//printf("[dataChannel-client] : IP=%s, Port=%d\n", inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));

	sendMsg("150 Openning data channel for directory listing of " + getCurPath() + CRLF);
	// getList
	if (getFileList() > 0) {
		cerr << "getFileList err " << endl;
	}

	retval = send(clientDataSock, dirList, strlen(dirList), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	cout << "send size : " << retval << endl;

	delete[] dirList;

	if (0 == closesocket(clientDataSock)) {
		ftpLog("clientDataSock close() ", LOG_INFO);
	}

	return 0;
}



int ControlHandler::getFileList() {   // for LIST command
	string targetPath = getRootPath();
	targetPath += getCurPath();

	struct tm ltm;
	dirList = new char[DIR_BUFSIZE] {""};
	char *tempList = new char[DIR_BUFSIZE];
	//ostringstream os;

	cout << "getFileList() targetPath : [" << targetPath << "]" << endl;

	ostringstream os;
	// 550 access is denied  처리해야함

		for (auto& p : fs::directory_iterator(targetPath)) {
			auto ftime = fs::last_write_time(p);
			time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			localtime_s(&ltm, &cftime);
			if (p.status().type() == fs::file_type::directory) {
				strftime(tempList, 1000, "%m-%d-%g  %R%p", &ltm);
				os << tempList << " <DIR> " << p.path().filename() << CRLF;
			}
			else if (p.status().type() != fs::file_type::directory) {
				strftime(tempList, 1000, "%m-%d-%g  %R%p", &ltm);
				os << tempList << " " << fs::file_size(p) << " " << p.path().filename() << CRLF;
			}
			else {
				cout << "unknown file type" << endl;
			}

		}
	string temp = os.str();
	strcpy_s(dirList, DIR_BUFSIZE, temp.c_str());
	cout << "---------------LIST-----------------------\n" << dirList << endl;

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