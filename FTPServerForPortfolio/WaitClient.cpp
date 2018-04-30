#include "WaitClient.h"





void WaitClients::Accepting(SOCKET &listen_sock) {

	//GetFiles();


	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		argList.sock = (void*)client_sock;
		cout << "===================================\n\n";
		printf("[TCP] client connected : IP=%s, Port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		fThread1 = (HANDLE)_beginthreadex(NULL, 0, ControlChannel, (void*)&argList, 0, NULL);
		if (fThread1 == NULL) { closesocket(client_sock); }
		else { CloseHandle(fThread1); }

		/*
		fThread2 = (HANDLE)_beginthreadex(NULL, 0, DataChannel, NULL, 0, NULL);
		if (fThread2 == NULL) { closesocket(client_sock); }
		else { CloseHandle(fThread2); }
		*/

	}
}



void WaitClients::GetFiles() {
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