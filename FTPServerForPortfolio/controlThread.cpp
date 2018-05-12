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


	int retval{ 0 };
	vector<string> argv;

	commandSeparator(argv, commands);


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
		createPORTSock(argv[1]);
	}
	else if (argv[0] == "LIST") {
		if (getCurPath()=="") {   
			ftpLog(LOG_ERROR,"command - LIST - getCur is empty");
			return 2;
		}
		sendList();

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
		
		if (argv[1][0] == '/') {  //  make full path
			if (argv[1][1] == NULL) {  // CWD '/' 贸府
				movePath("/");
			}
			else {
				movePath(argv[1] + "/");
			}
		}
		else if(argv[1][0] != '/'){  // make relative path
			addPath(argv[1] + "/");
		}
		ftpLog(LOG_TRACE, "Current Path : %s", getCurPath().c_str());
		sendMsg("250 CWD command successful." + CRLF);

	}
	else if (argv[0] == "RETR") { // file(s) are transffered from server to client.  RETR <FILENAME>
		ftpLog(LOG_TRACE, "parsed file name [%s]", argv[1].c_str());
		setFileName(argv[1]);

	
		sendFile();
	
	}
	else if (argv[0] == "REST") {
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
	int retval = 0;
	int addrlen{ 0 };
	int fileSize = 0;

	if (isActive == false) {
		addrlen = sizeof(dataClient_addr);
		clientDataSock = accept(dataListenSock, (SOCKADDR*)&dataClient_addr, &addrlen);
		if (clientDataSock == INVALID_SOCKET) {
			err_display("data accept()");
			closesocket(dataListenSock);
		}
	}
	
	fileSize = openFile();
	cout << "fileSize : " << fileSize << endl;
	
	sendMsg("125 Data connection already open; Transfer starting." + CRLF);


	/*
	while (!ifs->eof()) {
		fileSize = openFile();
		char *buf = new char[fileSize];
		ifs->read(buf, fileSize);
		cout << "fileSize : " << fileSize << endl;

		retval = send(clientDataSock, buf, fileSize, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		cout << "return : " << retval << endl;

		delete[] buf;
	}
	*/
		

	shutdown(clientDataSock, SD_BOTH);
	closesocket(clientDataSock);
	ifs->close();
	sendMsg("226 Transfer complete." + CRLF);
}



int ControlHandler::openFile() {
	int startPos{ 0 };
	int fileSize{ 0 };
	if (getRetrSize() > 0) {  // for REST
		startPos = getRetrSize() ;
		ifs->seekg(startPos, ios::end);
		fileSize = ifs->tellg();
		ifs->seekg(startPos, ios::beg);
		return fileSize;
	}
	

	string targetFile = getRootPath();
	targetFile += getCurPath() + getFileName(); 
	//ftpLog(LOG_TRACE, "targetPath : %s ", targetFile.c_str());

	ifs = new ifstream;
	//ifs->rdbuf()->open(targetFile, std::ios_base::binary);
	ifs->open(targetFile, std::ios_base::binary);
	if (!ifs) {
		sendMsg("550 file open failed" + CRLF);
		err_quit("file open failed");
	};

	size_t retval = 0;
	

		char *buf = new char[fileSize];
		size_t buffsize = 1 << 20;
	while (!ifs->eof()) {
	
		ifs->read(buf, buffsize);
		
		size_t count = ifs->gcount();
		if (!count) break;

		retval = send(clientDataSock, buf, 1, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		cout << "return : " << retval << endl;
		//delete[] buf;
	}
		delete[] buf;


	std::cout << " ifstream  file size: " << fileSize << endl;


	return fileSize;
}


void ControlHandler::sendList() {

	if (isActive == false) {
		pasvSendList();
	}
	else if(isActive == true) {
		portSendList();
	}

}


int ControlHandler::getFileList() {   // for LIST command
	string targetPath = getRootPath();
	targetPath += getCurPath();

	struct tm ltm;
	dirList = new char[DIR_BUFSIZE] {""};
	char *tempList = new char[DIR_BUFSIZE];

	ftpLog(LOG_TRACE, "getFileList() targetPath : [%s]", targetPath.c_str());

	ostringstream os;
	// 550 access is denied  贸府秦具窃
	try {
		for (auto& p : fs::directory_iterator(targetPath)) {
			if (p.path().filename() == "System Volume Information" && p.path().filename() == "$RECYCLE.BIN") continue;

			auto ftime = fs::last_write_time(p);
			time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			localtime_s(&ltm, &cftime);
			//printStatus(p, fs::status(p));

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
	catch (const fs::filesystem_error& e){
		cout << e.code().message() << endl;
	}

	string temp = os.str();
	strcpy_s(dirList, DIR_BUFSIZE, temp.c_str());
	delete[] tempList;
	cout << "size:" << sizeof(dirList) << endl;

	sendMsg("150 Openning data channel for directory listing of " + getCurPath() + CRLF);


	return strlen(dirList);
}

void ControlHandler::createPORTSock(string a) {
	string argv = a;
	string ip{ "" };
	int port{ 0 };
	int retval{ 0 };
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 1000;

	portDecoder(argv, ip, port, getServerIp());

	clientDataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientDataSock == INVALID_SOCKET) err_quit("data con socket()");

	bool optval = true;
	retval = setsockopt(clientDataSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt() - active");

	retval = setsockopt(clientDataSock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	ZeroMemory(&dataCon_addr, sizeof(dataCon_addr));   // for connect
	dataCon_addr.sin_family = AF_INET;
	dataCon_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	dataCon_addr.sin_port = htons(port);

	ZeroMemory(&dataListen_addr, sizeof(dataListen_addr));  // for bind
	dataListen_addr.sin_family = AF_INET;
	dataListen_addr.sin_addr.S_un.S_addr = inet_addr(getServerIp().c_str());
	//dataListen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	dataListen_addr.sin_port = htons(getActivePort());

	retval = bind(clientDataSock, (SOCKADDR*)&dataListen_addr, sizeof(dataListen_addr));  //bind
	if (retval == SOCKET_ERROR) err_quit("Active bind()");

	ftpLog(LOG_INFO, "[dataChannel-(Connecting...)] : IP=%s, Port=%d\n",
		inet_ntoa(dataListen_addr.sin_addr), ntohs(dataListen_addr.sin_port));

	retval = connect(clientDataSock, (SOCKADDR*)&dataCon_addr, sizeof(dataCon_addr));   // connect
	if (retval == SOCKET_ERROR) err_quit("Active connect()");

	isActive = true;

	sendMsg("200 PORT command successful." + CRLF);
	ftpLog(LOG_INFO, "[PORT.dataChannel - client] : IP = %s, Port = %d\n",
		inet_ntoa(dataCon_addr.sin_addr), ntohs(dataCon_addr.sin_port));


}


string ControlHandler::createPASVSock() {

	string port;
	int retval{ 0 };
	int len{ 0 };
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 1000;

	dataListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (dataListenSock == INVALID_SOCKET) err_quit("data socket()");

	bool optval = true;
	retval = setsockopt(dataListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	retval = setsockopt(dataListenSock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	ZeroMemory(&dataListen_addr, sizeof(dataListen_addr));
	dataListen_addr.sin_family = AF_INET;
	dataListen_addr.sin_addr.S_un.S_addr = inet_addr(getServerIp().c_str());
	//dataListen_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	dataListen_addr.sin_port = htons(0);

	retval = bind(dataListenSock, (SOCKADDR*)&dataListen_addr, sizeof(dataListen_addr));
	if (retval == SOCKET_ERROR) err_quit("data bind()");


	if (listen(dataListenSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}

	len = sizeof(dataListen_addr);
	getsockname(dataListenSock, (SOCKADDR*)&dataListen_addr, &len);
	ftpLog(LOG_INFO, "[dataChannel-(Listenning)] : IP=%s, Port=%d\n",
		inet_ntoa(dataListen_addr.sin_addr), ntohs(dataListen_addr.sin_port));

	int sinPort = ntohs(dataListen_addr.sin_port);
	string addr = inet_ntoa(dataListen_addr.sin_addr);


	return portEncoder(sinPort, addr);
}



void ControlHandler::pasvSendList() {
	int retval{ 0 };
	int listLen{ 0 };
	int addrlen{ 0 };

	addrlen = sizeof(dataClient_addr);
	clientDataSock = accept(dataListenSock, (SOCKADDR*)&dataClient_addr, &addrlen);
	if (clientDataSock == INVALID_SOCKET) {
		err_display("data accept()");
		closesocket(dataListenSock);
	}
	ftpLog(LOG_INFO, "[PASV.dataChannel - client] : IP = %s, Port = %d\n",
		inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));

	// getList
	listLen = getFileList();
	if (listLen <= 0) {
		ftpLog(LOG_ERROR, "LIST error");
		ftpLog(LOG_ERROR, "clientDataSock close() ");
	}
	retval = send(clientDataSock, dirList, strlen(dirList), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	delete[] dirList;
	shutdown(clientDataSock, SD_BOTH);
	closesocket(clientDataSock);

	sendMsg("226 Successfully transferred " + getCurPath() + "\"" + CRLF);

	ftpLog(LOG_INFO, "[dataChannel-client] : IP=%s, Port=%d is closed\n",
		inet_ntoa(dataClient_addr.sin_addr), ntohs(dataClient_addr.sin_port));
}



void ControlHandler::portSendList() {
	int retval{ 0 };
	int listLen{ 0 };
	int addrlen{ 0 };

	listLen = getFileList();

	if (listLen <= 0) {
		ftpLog(LOG_ERROR, "LIST error");
		ftpLog(LOG_ERROR, "clientDataSock close() ");
		delete[] dirList;
		shutdown(clientDataSock, SD_BOTH);
		closesocket(clientDataSock);
	}

	retval = send(clientDataSock, dirList, strlen(dirList), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	sendMsg("226 Successfully transferred " + getCurPath() + "\"" + CRLF);

	delete[] dirList;
	shutdown(clientDataSock, SD_BOTH);
	closesocket(clientDataSock);

	ftpLog(LOG_INFO, "[PORT.dataChannel-client] : IP=%s, Port=%d is closed\n",
		inet_ntoa(dataCon_addr.sin_addr), ntohs(dataCon_addr.sin_port));
}