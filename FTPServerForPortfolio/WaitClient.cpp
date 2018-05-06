#include "WaitClient.h"
#include "Utils.h"

static int accepted{ 0 };

void WaitClients::Starter()
{
	int retval{ 0 };
	if (setOrNot == false) {
		ftpLog(LOG_ERROR,"path not initialized");
		return;
	}

	//socket
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) err_quit("socket()");

	bool optval = true;
	retval = setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");


	ZeroMemory(&controlAddr, sizeof(controlAddr));
	controlAddr.sin_family = AF_INET;
	controlAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//if(inet_pton(AF_INET,"127.0.0.1",(&controlAddr.sin_addr.S_un.S_addr)) < 1) err_quit("bind() - inet_pton");
	controlAddr.sin_port = htons(SERVERPORT);

	//bind
	retval = bind(listenSock, (SOCKADDR*)&controlAddr, sizeof(controlAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen socket
	if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}
	ftpLog(LOG_INFO,"==================================================");
	ftpLog(LOG_INFO,"[control@Channel-server]  : IP=%s, Port=%d\n", 
		inet_ntoa(controlAddr.sin_addr), ntohs(controlAddr.sin_port));


	accepting(listenSock);
}

void WaitClients::accepting(SOCKET &listen_sock) {


	while (1) {
		addrlen = sizeof(clientaddr);
		controlSock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (controlSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		argList.sock = (void*)controlSock; //with root path
		ftpLog(LOG_INFO,"[accepted-control] : IP=%s, Port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		ftpLog(LOG_INFO,"=====================================");

		fThread1 = (HANDLE)_beginthreadex(NULL, 0, controlHandler, (void*)&argList, 0, NULL);
		if (fThread1 == NULL) { closesocket(controlSock); }
		else { CloseHandle(fThread1); }


	}
}


