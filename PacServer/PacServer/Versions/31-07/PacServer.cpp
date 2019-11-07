// PacServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PacServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

UINT RunServer(LPVOID pParam);
UINT RunClient(LPVOID pParam);

//The windows communication class object
WComm wcomm;
// The one and only application object
CWinApp theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		printf( "Press ESCAPE to terminate PacServer\r\n" );
		AfxBeginThread(RunServer, 0);
		while( _getch() != 27 ){}
	}

	return nRetCode;
}


UINT RunServer(LPVOID pParam)
{
	// Start Server Daemon
	wcomm.startServer(27015);
	printf("Server Started!!!\n");

	while(true)
	{
		// Wait until a client connects, print his IP to console
		wcomm.cliIP = wcomm.returnClient();
		AfxBeginThread(RunClient, 0 );
		
		printf( wcomm.cliIP );
		printf(" : Client Connected!\n");
	}
	wcomm.closeConnection();
	wcomm.~WComm();

	return 0;
}

UINT RunClient(LPVOID pParam)
{
	//The windows communication class object
	WComm w = wcomm;
	char dname[32] = "";
	FILE *outFile;

	// Work with client
	while(true)
	{
		string path = "C:/PacPlayd/";
		path += w.cliIP;
		_mkdir( path.c_str() );
		path += "/";
		char rec[32] = "";
		w.recvData(rec,32); w.sendData("OK");

		if(strcmp(rec,"") != 0)
		{
			printf("Instruction: " ); printf(rec); printf("\n");
		}
		
		if(strcmp(rec,"DirSend") == 0)
		{
			w.recvData(dname, 32); w.sendData("OK");
			path += dname;
			_mkdir( path.c_str() );
			printf("Directory Received!\n");
		}
		else if(strcmp(rec,"FileSend") == 0)
		{
			char fname[32] = "";
			w.recvData(fname, 32); w.sendData("OK");
			path += dname;
			path += fname;
			fopen_s( &outFile, path.c_str(), "wt" );
			printf("Filename Received!\n");
		}
		else if(strcmp(rec,"DataSend") == 0)
		{
			char recData[500000] = "";
			w.recvData(recData, 500000); w.sendData("OK");
			
			if(outFile == NULL)
				fopen_s( &outFile, "recData.csv", "wt" );

			fputs(recData, outFile);
			fflush(outFile);
			fclose(outFile);
			printf("Data Received!\n");
		}
		else if(strcmp(rec,"EndConnection") == 0)
		//We're doing one-shot clients so break automatically after transfer
			break;
	}
	printf("Connection Ended......\n");
	// Disconnect client
	w.closeConnection();

	return 0;
}
//End code - add one more empty line:
