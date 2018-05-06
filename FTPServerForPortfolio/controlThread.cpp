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

	retval = send(controlSock, resBuf, strlen(resBuf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	while (1) {
		retval = recv(controlSock, commands, COM_BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}
		commands[retval] = '\0';


		if (commandsHandler() == 2) {
			cerr << "commandsHandler exit()";
		}
		
	}
	return 0;
}



int ControlHandler::commandsHandler() {

	int resLen = strlen(commands);
	int retval;
	char * context = NULL;
	vector<string> argv;
	
	int cnt = 0;

	if (commands[resLen - 2] == '\r' && commands[resLen - 1] == '\n') {  // delete to cargviage 
		commands[strlen(commands) - 2] = '\0';
	}

	char *token = strtok_s(commands, " ", &context);
	argv.push_back(commands);
	argv.push_back(context);


	ftpLog(LOG_INFO, "[%d]commands : %s, argc : %d, argv : %s", countForDebug, commands, argv.size(), context);



	if (argv[0] == "USER") {
		ftpLog(LOG_DEBUG, "230 logged in");
		sendMsg("230 logged in" + CRLF);
	}
	else if (argv[0] == "PWD") {
		ftpLog(LOG_INFO, "Current Path : %s", getCurPath().c_str());
		sendMsg("257 \"" + getCurPath() + "\" is current directory." + CRLF);
		ftpLog(LOG_DEBUG, "257 [%s] is current directory.", getCurPath().c_str());
	}
	else if (argv[0] == "TYPE") {
		//override Type
		sendMsg("200 Type set to I(binary mode)" + CRLF);
		ftpLog(LOG_DEBUG,"200 Type set to I");
	}
	else if (argv[0] == "SYST") {
		sendMsg("215 Windows_10" + CRLF);
		ftpLog(LOG_DEBUG,"215 Windows_10");
	}
	else if (argv[0] == "FEAT") {
		sendMsg("211 END" + CRLF);
		ftpLog(LOG_DEBUG,"211 END");
	}
	else if (argv[0] == "PASV") {
		sendMsg("227 Entering Passive Mode " + createDataSock() + CRLF);
		ftpLog(LOG_DEBUG, "227 Entering Passive Mode ");
	}
	else if (argv[0] == "LIST") {
		if (getCurPath()=="") {   
			ftpLog(LOG_ERROR,"command - LIST - getCur is empty");
			return 2;
		}
		sendList();

		sendMsg("226 Successfully transferred " + getCurPath() + "\"" + CRLF);
		ftpLog(LOG_DEBUG,"command - LIST - getCur is empty");
	}
	else if (argv[0] == "CDUP") {
		string cdup = getCurPath();
		cdup.erase(cdup.length() - 1, 1);
		int pos = cdup.rfind("/", cdup.length() - 1);
		if (pos == 0) {
			movePath(cdup.erase(1, cdup.length()));
			sendMsg("250 CDUP Commands successful." + CRLF);
			ftpLog(LOG_INFO, "250 CDUP Commands successful.");
			return 1;
		}
		movePath(cdup.erase(pos, cdup.length()));
		sendMsg("250 CDUP Commands successful." + CRLF);
		ftpLog(LOG_INFO, "250 CDUP Commands successful.");
	}
	else if (argv[0] == "CWD") {
	
		ftpLog(LOG_DEBUG, "CWD argc : %d", argv.size());
		for (int i = 0; i < argv.size(); i++) {
			ftpLog(LOG_DEBUG, "CWD[%d] : %s", i, argv[i].c_str());
		}
		
		if (argv[1][0] == '/') {  // 클라이언트의 캐시로 이미 목록을 가져간것에서 더 하위로 옮길때는 클라가 full path를 던져줌
			if (argv[1][1] == NULL) {  // CWD '/' 처리
				movePath("/");
			}
			else {
				movePath(argv[1] + "/");
			}
		}
		else if(argv[1][0] != '/'){  // relative path
			addPath(argv[1] + "/");
		}
		ftpLog(LOG_INFO, "Current Path : %s", getCurPath().c_str());
		sendMsg("250 CWD command successful." + CRLF);
		ftpLog(LOG_DEBUG, "250 CWD Commands successful.");

	}
	else if (argv[0] == "RETR") {
	//else if (!strcmp(argv[0], "RETR")) {   // file(s) are transffered from server to client.  RETR <FILENAME>
		ftpLog(LOG_DEBUG, "RETR argc : %d", argv.size());
	
		ftpLog(LOG_DEBUG, "parsed file name [%s]", argv[1].c_str());
		setFileName(argv[1]);

		sendMsg("125 Successfully transferred" + CRLF);
		ftpLog(LOG_DEBUG,"125 Successfully transferred.");

		sendFile();
		sendMsg("226 Transfer complete." + CRLF);
		ftpLog(LOG_DEBUG,"226 Transfer complete.");
	}


	else {
		ftpLog(LOG_ERROR, "%s : command not defined", argv[0].c_str());
		countForDebug++;
		return 2;
	}

	countForDebug++;
	ftpLog(LOG_DEBUG,"=============================================");

	return 1;
}


void ControlHandler::sendFile() {
	int retval{ 0 };
	int addrlen{ 0 };
	int fileSize{ 0 };

	addrlen = sizeof(dataClient_addr);

	clientDataSock = accept(dataSock, (SOCKADDR*)&dataClient_addr, &addrlen);
	if (clientDataSock == INVALID_SOCKET) {
		err_display("data accept()");
		closesocket(dataSock);
	}

	string buf;
	fileSize = openFile();
	if (fileSize > (int)buf.max_size()) {
		ftpLog(LOG_DEBUG,"exceed a max file size");
		return;
	}
	buf.resize(fileSize);
	
	
	ifs->read(&buf[0], buf.size());

	retval = send(clientDataSock, buf.c_str(), fileSize, 0); /// buffer사이즈 초과 처리해야함.
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
		
	closesocket(clientDataSock);

	ifs->close();
}



int ControlHandler::openFile() {
	int fileSize{ 0 };
	string targetFile = getRootPath();
	targetFile += getCurPath(); // space 처리 잘해야함. space 두개 세개일수도 있음.
	targetFile += getFileName();
	//cout << "targetPath : " << targetFile << endl;
	ifs = new ifstream;
	ifs->open(targetFile, std::ios_base::binary);
	if (!ifs) {
		sendMsg("550 file open failed" + CRLF);
		err_quit("file open failed");
	};
	//cout << "content : " << ifs->rdbuf();
	ifs->seekg(0, ios::end);
	fileSize = (int)ifs->tellg();
	//cout << "file size : " << fileSize << endl;
	ifs->seekg(0, ios::beg);
	return fileSize;
}



void ControlHandler::sendList() {

	int retval{ 0 };
	int addrlen{ 0 };
	int listLen{ 0 };

	addrlen = sizeof(dataClient_addr);

	clientDataSock = accept(dataSock, (SOCKADDR*)&dataClient_addr, &addrlen);
	if (clientDataSock == INVALID_SOCKET) {
		err_display("data accept()");
		closesocket(dataSock);
	}
	ftpLog(LOG_INFO, "[dataChannel - client] : IP = %s, Port = %d\n", 
		inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));

	sendMsg("150 Openning data channel for directory listing of " + getCurPath() + CRLF);
	// getList
	listLen = getFileList();
	if (listLen <= 0) {
		ftpLog(LOG_ERROR,"LIST error");
		ftpLog(LOG_ERROR,"clientDataSock close() ");
	}
		retval = send(clientDataSock, dirList, strlen(dirList), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		delete[] dirList;
		closesocket(clientDataSock);
	ftpLog(LOG_INFO,"[dataChannel-client] : IP=%s, Port=%d is closed\n",
		inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));

}


int ControlHandler::getFileList() {   // for LIST command
	string targetPath = getRootPath();
	targetPath += getCurPath();

	struct tm ltm;
	dirList = new char[DIR_BUFSIZE] {""};
	char *tempList = new char[DIR_BUFSIZE];

//	cout << "getFileList() targetPath : [" << targetPath << "]" << endl;

	ostringstream os;
	// 550 access is denied  처리해야함
	// 전부(system volume information)까지 보이므로.. 안보이게 처리해야함.

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
	//cout << "---------------LIST-----------------------\n" << dirList << endl;
	delete[] tempList;

	return strlen(dirList);
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

	/*
	int sendBufSize{ SEND_BUFSIZE };
	retval = setsockopt(dataSock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
	*/

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
	ftpLog(LOG_INFO, "[dataChannel-server(Listen)] : IP=%s, Port=%d\n",
		inet_ntoa(dataServer_addr.sin_addr), ntohs(dataServer_addr.sin_port));

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