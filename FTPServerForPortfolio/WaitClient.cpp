#include "WaitClient.h"

unsigned WINAPI controlHandler(void *arg)
{
	ControlHandler t = new ControlHandler(arg);
	t.controlActivate();
	return 0;
}


void WaitClients::accepting(SOCKET &listen_sock) {

	//GetFiles();

	while (1) {
		addrlen = sizeof(clientaddr);
		controlSock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (controlSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		argList.sock = (void*)controlSock;
		
		printf("[controlChannel-client] : IP=%s, Port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		cout << "===================================\n\n\n\n";

		fThread1 = (HANDLE)_beginthreadex(NULL, 0, controlHandler, (void*)&argList, 0, NULL);
		if (fThread1 == NULL) { closesocket(controlSock); }
		else { CloseHandle(fThread1); }


	}
}



void WaitClients::getFiles() {
	for (auto& p : fs::recursive_directory_iterator(path)) {

		if (p.status().type() == fs::file_type::directory) {
			fs::create_directories(p.path().string().replace(0, 1, "e"));
		}
		else if (p.status().type() != fs::file_type::directory) {
			cout << "path : " << p.path() << ", size : " << file_size(p) << " byte" << endl;
			fileNames.push_back(p.path().string());
		}
		else {
			cout << "unknown type!" << endl;
			return;
		}
	}
}