// Chat_Clnt_W.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
void ErrorHandling(const char *msg);
bool recvEnd = false;
HANDLE gHSndThread,gHRcvThread;
char name[256] = "[DEFAULT]";
char msg[256];

int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;
	if (argc != 4) {
		cout << "Usage : " << argv[0] << "<IP> <port> <name>" << endl;
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");
	}

	sprintf_s(name, "[%s]", argv[3]);
	hSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
		ErrorHandling("connect() error!");
	}

	hSndThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);
	gHSndThread = hSndThread;
	gHRcvThread = hRcvThread;
	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);
	cout << "Terminated By Server";
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void * arg) {
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[256 + 256];
	while (true)
	{
		fgets(msg, 256, stdin);
		
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
			closesocket(hSock);
			TerminateThread(gHRcvThread, 0);
			break;
		}
		sprintf_s(nameMsg, "%s : %s", name, msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg) {
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[256+ 256];
	int strLen;
	while (true)
	{
		strLen = recv(hSock, nameMsg, 256 +256 - 1, 0);
		if (strLen == -1) {
			
			closesocket(hSock);
			TerminateThread(gHSndThread, 0);
			break;
		}
		nameMsg[strLen] = 0;
		fputs(nameMsg, stdout);
	}
	return 0;
}


void ErrorHandling(const char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}