#include "FtpServer.h"
#include "Utils.h"

static int controlId = 0;

void FtpServer::Starter()
{
	int retval{ 0 };
	if (setOrNot == false) {
		err_quit("Path Not Set");
		ftpLog(LOG_ERROR, "Path Not Set");
		return;
	}

	//socket
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) err_quit("socket()");

	bool optval = true;
	retval = setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	bool bEnable = true;
	retval = setsockopt(listenSock, SOL_SOCKET, SO_KEEPALIVE, (char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");


	ZeroMemory(&controlAddr, sizeof(controlAddr));
	controlAddr.sin_family = AF_INET;
	controlAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	controlAddr.sin_port = htons(controlPort);

	//bind
	retval = bind(listenSock, (SOCKADDR*)&controlAddr, sizeof(controlAddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen socket
	if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}
	ftpLog(LOG_INFO, "==================================================");
	ftpLog(LOG_INFO, "[Server-ControlChannel] : IP=%s, Port=%d",
		inet_ntoa(controlAddr.sin_addr), ntohs(controlAddr.sin_port));


	accepting(listenSock);
}

void FtpServer::accepting(SOCKET &listen_sock) {
	int addrlen{ 0 };

	while (1) {
		addrlen = sizeof(clientaddr);
		controlSock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (controlSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
	
		addrlen = sizeof(serverIp);
		getsockname(controlSock, (SOCKADDR*)&serverIp, &addrlen);
		ftpLog(LOG_INFO, "[(serverIp)] : IP=%s, Port=%d\n",
			inet_ntoa(serverIp.sin_addr), ntohs(serverIp.sin_port));
		
		ftpLog(LOG_INFO, "%d [Server-Accept] From...  IP=%s, Port=%d", controlId,
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		ftpLog(LOG_INFO, "==================================================");


		// pass to ControlHandler class
		argList.id = controlId++;
		argList.sock = (void*)controlSock; //with root path
		argList.activePort = getActivePort();
		argList.serverIP = inet_ntoa(serverIp.sin_addr);

		fThread1 = (HANDLE)_beginthreadex(NULL, 0, controlHandler, (void*)&argList, 0, NULL);
		if (fThread1 == NULL) { closesocket(controlSock); }
		else { CloseHandle(fThread1); }


	}
}


