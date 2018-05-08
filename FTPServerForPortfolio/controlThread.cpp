#include "ControlHandler.h"
#include "Utils.h"

unsigned WINAPI controlHandler(void *arg)
{
	ControlHandler t = new ControlHandler(arg);
	t.controlActivate();
	return 0;
}

int ControlHandler::sendMsg(const string msg) {
	ftpLog(LOG_INFO, msg.c_str());
	int retval = send(controlSock, msg.c_str(), msg.length(), 0);
	if (retval == SOCKET_ERROR) {
		err_display("sendMsg()");
	}

	return retval;
}


int ControlHandler::controlActivate() {
	ftpLog(LOG_DEBUG, "ControlActivated");
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

		commandsHandler();
		
	}
	return 0;
}



int ControlHandler::commandsHandler() {

	int resLen = strlen(commands);
	int retval{ 0 };
	vector<string> argv;
	
	if (commands[resLen - 2] == '\r' && commands[resLen - 1] == '\n') {  // delete to cargviage 
		commands[strlen(commands) - 2] = '\0';
	}

	char * context = NULL;
	char *token = strtok_s(commands, " ", &context);
	argv.push_back(commands);
	argv.push_back(context);


	ftpLog(LOG_TRACE, "commands : %s, argc : %d, argv : %s", commands, argv.size(), context);



	if (argv[0] == "USER") {
		sendMsg("230 logged in" + CRLF);
	}
	else if (argv[0] == "PWD") {
		ftpLog(LOG_TRACE, "Current Path : %s", getCurPath().c_str());
		sendMsg("257 \"" + getCurPath() + "\" is current directory." + CRLF);
	}
	else if (argv[0] == "TYPE") {
		//override Type
		sendMsg("200 Type set to I(binary mode)" + CRLF);
	}
	else if (argv[0] == "SYST") {
		sendMsg("215 Windows_10" + CRLF);
	}
	else if (argv[0] == "FEAT") {
		sendMsg("211 END" + CRLF);
	}
	else if (argv[0] == "PASV") {
		sendMsg("227 Entering Passive Mode " + createPASVSock() + CRLF);
	}
	else if (argv[0] == "PORT") {
		for (unsigned int i = 0; i < argv.size(); i++) {
			ftpLog(LOG_TRACE, "PORT[%d] : %s", i, argv[i].c_str());
		}
		createPORTSock(argv[1]);
		isActive = true;
		sendMsg("200 PORT command successful." + CRLF);
	}
	else if (argv[0] == "LIST") {
		if (getCurPath()=="") {   
			ftpLog(LOG_ERROR,"command - LIST - getCur is empty");
			return 2;
		}
		sendList();
		sendMsg("226 Successfully transferred " + getCurPath() + "\"" + CRLF);
	}
	else if (argv[0] == "CDUP") {
		string cdup = getCurPath();
		cdup.erase(cdup.length() - 1, 1);
		int pos = cdup.rfind("/", cdup.length() - 1);
		if (pos == 0) {
			movePath(cdup.erase(1, cdup.length()));
			sendMsg("250 CDUP Commands successful." + CRLF);
			return 1;
		}
		movePath(cdup.erase(pos, cdup.length()));
		sendMsg("250 CDUP Commands successful." + CRLF);
	}
	else if (argv[0] == "CWD") {
	
		ftpLog(LOG_TRACE, "CWD argc : %d", argv.size());
		for (unsigned int i = 0; i < argv.size(); i++) {
			ftpLog(LOG_TRACE, "CWD[%d] : %s", i, argv[i].c_str());
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
		ftpLog(LOG_TRACE, "Current Path : %s", getCurPath().c_str());
		sendMsg("250 CWD command successful." + CRLF);

	}
	else if (argv[0] == "RETR") { // file(s) are transffered from server to client.  RETR <FILENAME>
		ftpLog(LOG_TRACE, "RETR argc : %d", argv.size());
	
		ftpLog(LOG_TRACE, "parsed file name [%s]", argv[1].c_str());
		setFileName(argv[1]);

		sendMsg("125 Successfully transferred" + CRLF);

		sendFile();
	
	}
	else if (argv[0] == "REST") {
		ftpLog(LOG_TRACE, "REST argv : %d", argv.size());
		ftpLog(LOG_TRACE, "REST startSize : %s", argv[1].c_str());
		setRetrSize(stoi(argv[1]));
		sendMsg("350 Restarting at " + argv[1] + CRLF);
	}

	else {
		ftpLog(LOG_ERROR, "%s : command not defined", argv[0].c_str());
		//return 2;
	}

	return 1;
}


void ControlHandler::sendFile() {
	int retval{ 0 };
	int addrlen{ 0 };
	int fileSize{ 0 };

	if (isActive == false) {
		addrlen = sizeof(dataClient_addr);
		clientDataSock = accept(dataListenSock, (SOCKADDR*)&dataClient_addr, &addrlen);
		if (clientDataSock == INVALID_SOCKET) {
			err_display("data accept()");
			closesocket(dataListenSock);
		}
	}

	string buf;
	fileSize = openFile();
	if (fileSize > (int)buf.max_size()) {
		ftpLog(LOG_FETAL,"exceed a max file size");
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
	sendMsg("226 Transfer complete." + CRLF);
}



int ControlHandler::openFile() {
	int startPos{ 0 };
	if (getRetrSize() > 0) {
		startPos = getRetrSize() ;
	}

	int fileSize{ 0 };
	string targetFile = getRootPath();
	targetFile += getCurPath(); 
	targetFile += getFileName();
	//cout << "targetPath : " << targetFile << endl;
	ifs = new ifstream;
	ifs->open(targetFile, std::ios_base::binary);
	if (!ifs) {
		sendMsg("550 file open failed" + CRLF);
		err_quit("file open failed");
	};
	//cout << "content : " << ifs->rdbuf();
	ifs->seekg(startPos, ios::end);
	fileSize = (int)ifs->tellg();
	//cout << "file size : " << fileSize << endl;
	ifs->seekg(startPos, ios::beg);
	return fileSize;
}



void ControlHandler::sendList() {

	int retval{ 0 };
	int addrlen{ 0 };
	int listLen{ 0 };

	if (isActive == false) {
		addrlen = sizeof(dataClient_addr);
		clientDataSock = accept(dataListenSock, (SOCKADDR*)&dataClient_addr, &addrlen);
		if (clientDataSock == INVALID_SOCKET) {
			err_display("data accept()");
			closesocket(dataListenSock);
		}
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

	ftpLog(LOG_INFO, "getFileList() targetPath : [%s]", targetPath.c_str());

	ostringstream os;
	// 550 access is denied  처리해야함
	// 전부(system volume information)까지 보이므로.. 안보이게 처리해야함.

	try {
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
				ftpLog(LOG_WARN, "Unknown file type");
			}
		}
	}
	catch (const std::exception& e) {
		ftpLog(LOG_FETAL, e.what());
	}

	string temp = os.str();
	strcpy_s(dirList, DIR_BUFSIZE, temp.c_str());
	//cout << "---------------LIST-----------------------\n" << dirList << endl;
	delete[] tempList;

	return strlen(dirList);
}

void ControlHandler::createPORTSock(string a) {
	string argv = a;
	string ip{ "" };
	int port{ 0 };
	int retval{ 0 };

	replacePORT(argv, ip, port);

	clientDataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientDataSock == INVALID_SOCKET) err_quit("data con socket()");

	bool optval = true;
	retval = setsockopt(clientDataSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt() - active");

	ZeroMemory(&dataCon_addr, sizeof(dataCon_addr));
	dataCon_addr.sin_family = AF_INET;
	dataCon_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	dataCon_addr.sin_port = htons(port);

	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.S_un.S_addr = inet_addr("192.168.219.101");
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(ACTIVEPORT);
	retval = bind(clientDataSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("Active bind()");


	retval = connect(clientDataSock, (SOCKADDR*)&dataCon_addr, sizeof(dataCon_addr));
	if (retval == SOCKET_ERROR) err_quit("Active connect()");

	ftpLog(LOG_INFO, "Active Mode Working");

}


string ControlHandler::createPASVSock() {

	string port;
	int retval{ 0 };
	int len{ 0 };

	dataListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dataListenSock == INVALID_SOCKET) err_quit("data socket()");

	bool optval = true;
	retval = setsockopt(dataListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	/*
	int sendBufSize{ SEND_BUFSIZE };
	retval = setsockopt(dataListenSock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
	*/

	ZeroMemory(&dataListen_addr, sizeof(dataListen_addr));
	dataListen_addr.sin_family = AF_INET;
	//dataListen_addr.sin_addr.S_un.S_addr = inet_addr("192.168.219.101");
	dataListen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	dataListen_addr.sin_port = htons(0);

	retval = bind(dataListenSock, (SOCKADDR*)&dataListen_addr, sizeof(dataListen_addr));
	if (retval == SOCKET_ERROR) err_quit("data bind()");


	if (listen(dataListenSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}

	len = sizeof(dataListen_addr);
	getsockname(dataListenSock, (SOCKADDR*)&dataListen_addr, &len);
	ftpLog(LOG_INFO, "[dataChannel-server(Listen)] : IP=%s, Port=%d\n",
		inet_ntoa(dataListen_addr.sin_addr), ntohs(dataListen_addr.sin_port));

	int sinPort = ntohs(dataListen_addr.sin_port);
	int highBit = (sinPort >> 8) & 255;
	int lowBit = sinPort & 255;

	port = "(";
	port += inet_ntoa(dataListen_addr.sin_addr);
	port += "," + to_string(highBit) + "," + to_string(lowBit) + " )";

	replaceString(port, ".", ",");

	port += ".";

	return port;
}