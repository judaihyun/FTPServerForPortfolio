#include "WaitClient.h"


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


		fThread1 = (HANDLE)_beginthreadex(NULL, 0, ControlChannel, &argList, 0, NULL);
		if (fThread1 == NULL) { closesocket(client_sock); }
		else { CloseHandle(fThread1); }

		/*
		fThread2 = (HANDLE)_beginthreadex(NULL, 0, DataChannel, (void*)client_sock, 0, NULL);
		if (fThread2 == NULL) { closesocket(client_sock); }
		else { CloseHandle(fThread2); }
		*/

	}
}

unsigned WINAPI ControlChannel(void* arg) {
	passToThread argList = *((passToThread*)arg);
	SOCKET controlChannel = (SOCKET)argList.sock;
	string path = argList.path;
	SOCKADDR_IN client;
	int retval{ 0 };
	int addrlen{ 0 };
	char buf[BUFSIZE + 1]{ "220 FTP Test Serivce. \r\n" };

	addrlen = sizeof(client);
	getpeername(controlChannel, (SOCKADDR*)&client, &addrlen);

	retval = send(controlChannel, buf, strlen(buf), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	printf("[Send] %s \n", buf);

	while (1) {
				
		char response[40]{ '\0' };
		retval = recv(controlChannel, response, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}

		response[retval] = '\0';
		printf("[Received] %s\n", response);
		
		Parser(response, buf, path);

		retval = send(controlChannel, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
		printf("[Send] %s \n", buf);
	}

	closesocket(controlChannel);

	return 0;
}

void Parser(char *res, char *buf, string p) {
	//cout << "response : " << res << endl;
	string CRLF = " \r\n";
	string path = "\"" + p + "\"";
	int resLen = strlen(res);
	
	char * context = NULL;
	char *arr[5];
	int cnt = 0;

	if (res[resLen - 2] == '\r' && res[resLen - 1] == '\n') {  // delete to carriage 
		res[strlen(res) - 2] = '\0';
	}

	char *token = strtok_s(res, " ", &context);
	while (token!=NULL) {
		//cout << "arr[" << cnt << "] : [" << token  << "]" << endl;
		arr[cnt++] = token;
		token = strtok_s(NULL, " ", &context);
	}

	if (!strcmp(arr[0], "USER")) {
		string temp = "230 logged in" + CRLF;
		strcpy_s(buf, BUFSIZE, temp.c_str());
	}
	else if (!strcmp(arr[0], "PWD")) {
		string temp = "257 " + path +" is current directory." + CRLF;
		strcpy_s(buf, BUFSIZE, temp.c_str());
	}
	else if (!strcmp(arr[0], "TYPE")) {  
		string temp = "200 Type set to I(binary mode)" + CRLF;
		strcpy_s(buf, BUFSIZE, temp.c_str());
	}
	else if (!strcmp(arr[0], "PASV")) {  
		string temp = "227 Entering Passive Mode (binary mode)" + CRLF;
		strcpy_s(buf, BUFSIZE, temp.c_str());

	}

}

unsigned WINAPI DataChannel(void* arg) {

	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN client;
	int retval{ 0 };
	int addrlen{ 0 };
	char buf[BUFSIZE + 1]{ NULL };

	addrlen = sizeof(client);
	getpeername(client_sock, (SOCKADDR*)&client, &addrlen);
	printf("[DataChannel] client connected : IP=%s, PORT=%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	
	while (1) {
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}

		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
	}
	

	closesocket(client_sock);

	return 0;
}



