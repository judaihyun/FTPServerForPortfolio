#include "FTPServer.h"



void FTPServer::Starter() {
	if (setOrNot == false) {
		cerr << "path not set \n";
		return;
	}

	int retval{ 0 };
	//socket
	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	bool optval = true;
	retval = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");


	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//if(inet_pton(AF_INET,"127.0.0.1",(&serveraddr.sin_addr.S_un.S_addr)) < 1) err_quit("bind() - inet_pton");
	serveraddr.sin_port = htons(SERVERPORT);

	//bind
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen socket
	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
		err_quit("listen()");
	}
	cout << "===================================\n";
	printf("[controlChannel-server]  : IP=%s, Port=%d\n",
		inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));

	clients.setPath(path);
	clients.accepting(listen_sock);

}

