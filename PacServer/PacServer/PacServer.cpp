// PacServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PacServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

UINT RunServer(LPVOID pParam);
UINT RunAClient(LPVOID pParam);
int sendData(SOCKET socket, char *sendbuf);
int recvData(SOCKET socket, char *recvbuf, int size);
void print(const char* out);

// The one and only application object
CWinApp theApp;
SOCKET server;
char* clip = "";
FILE *myLog;
u_short PORTNUM = 4276;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	fopen_s(&myLog, "../ServerLog.txt", "wt");
	if( argc == 2 )
		PORTNUM = (u_short)atoi((char*)argv[0]);

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		print( "Press ESCAPE to terminate PacServer\r\n" );
		AfxBeginThread(RunServer, 0);
		while( _getch() != 27 ){}
	}
	closesocket(server);
	WSACleanup();
	fflush(myLog);
	fclose(myLog);

	return nRetCode;
}

/**
 *	Multithread server code from code project
 */
UINT  RunServer(LPVOID pParam)
{		
	WSADATA wsaData;
	sockaddr_in local;
	int wsaret = WSAStartup(0x101, &wsaData);
	if(wsaret != 0)
		return 0;

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(PORTNUM);
	server = socket(AF_INET,SOCK_STREAM,0);
	if(server == INVALID_SOCKET)
		return 0;

	if( bind(server, (sockaddr*)&local, sizeof(local)) != 0)
		return 0;

	if(listen(server,10) != 0)
		return 0;

	SOCKET client;
	sockaddr_in from;
	int fromlen = sizeof(from);

	while(true)
	{
		client = accept(server,	(struct sockaddr*)&from, &fromlen);
		clip = inet_ntoa(from.sin_addr);
		print( clip );
		print(" : Client Connected!\n");
		for(int i = 0; i < fromlen; i++)
			if(clip[i] == '.')
				clip[i] = '_';
		AfxBeginThread(RunAClient, (LPVOID)client);
	}	

	return 0;
}

UINT RunAClient(LPVOID pParam)
{
	SOCKET client = (SOCKET)pParam;
	string dir = "../";
	//Get the client IP, make a directory, add a final slash to the absolute path
	dir += clip;
print( dir.c_str() ); print("\n\n");
	_mkdir( dir.c_str() );
	dir += "/";
	
	char rec[32] = "";
	char dname[32] = "";
	FILE *outFile;
	char* IP = clip;
	
	// Work with client
	while(true)
	{
		string path = dir;
		
		if(recvData(client, rec, 32) == SOCKET_ERROR)
			break;

		print(IP); print(" sent instruction: " ); print(rec); print("\n");
		
		//Parse Commands from client - process data unless command is EndConnection
		if(strcmp(rec, "DirSend") == 0)
		{
			sendData(client, "OK1");
			//Get - a directory name for the session, in the form of Yr_Dy_Hr_Mn_Sc, no slashes
			recvData(client, dname, 32); sendData(client, "OK2");
			dir += dname;
print( dir.c_str() ); print("\n\n");
			if(_mkdir( dir.c_str() ) != 0)
				print("Time based dir failed; Path is bad\n\n");
			else
				print("Directory Received!\n\n");
		}
		else if(strcmp(rec, "FileSend") == 0)
		{
			sendData(client, "OK1");
			char fname[32] = "";
			//Get - a filename in the form of /Level-'X'.csv
			recvData(client, fname, 32); sendData(client, "OK2");
			path += fname;
print( path.c_str() ); print("\n\n");
			if(fopen_s( &outFile, path.c_str(), "wt" ) == 0)
				print("Filename Received!\n\n");
			else
				print("Filename Invalid! Not opened!\n\n");
		}
		else if(strcmp(rec,"DataSend") == 0)
		{
			sendData(client, "OK1");
			char recData[500000] = "";
			recvData(client, recData, 500000); sendData(client, "OK2");
			
			if(outFile != NULL)
			{
				fputs(recData, outFile);
				fflush(outFile);
				fclose(outFile);
				print("Data Received!\n\n");
			}
		}
		else if(strcmp(rec,"EndConnection") == 0)
		{
			sendData(client, "END");
			//We're doing one-shot clients so break automatically after transfer
			break;
		}else
			sendData(client, "FAIL");

	}//end while
	print("Connection Ended......\n\n");
	// Disconnect client
	closesocket(client);

	return 0;
}

int sendData(SOCKET socket, char *sendbuf)
{
	return send( socket, sendbuf, strlen(sendbuf), 0 );
}


int recvData(SOCKET socket, char *recvbuf, int size)
{
	int sz = recv( socket, recvbuf, size, 0 );
	recvbuf[sz] = '\0';
	return sz;
}

void print(const char* out)
{
	fputs(out, myLog);
	printf(out);
}
/*End code - add one more empty line:*/
