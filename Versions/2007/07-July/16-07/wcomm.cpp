#include "wcomm.h"

WSADATA wsaData;
SOCKET m_socket;
SOCKET m_backup;
sockaddr_in con;
SOCKET AcceptSocket;

WComm::WComm()
{
    // Initialize Winsock.
	if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR )
	{
		//printf("Error at WSAStartup()\n");
	}

    // Create a socket.
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_socket == INVALID_SOCKET ) {
        //printf( "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return;
    }
	m_backup = m_socket;
}

WComm::~WComm(){WSACleanup();}

void WComm::connectServer(char *ip, int port)
{
    // Connect to a server.
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( ip );
    con.sin_port = htons( port );

    if ( connect( m_socket, (SOCKADDR*) &con, sizeof(con) ) == SOCKET_ERROR) {
        //printf( "Failed to connect.\n" );
        WSACleanup();
        return;
    }
}

int WComm::sendData(const char *sendbuf)
{
	return send( m_socket, sendbuf, strlen(sendbuf), 0 );
}


int WComm::recvData(char *recvbuf,int size)
{
	int sz = recv( m_socket, recvbuf, size, 0 );
	recvbuf[sz] = '\0';
	return sz;
}

/** /
void WComm::closeConnection()
{
	closesocket(m_socket);
	m_socket = m_backup;
}

/** /
void WComm::fileSend(char *fpath)
{
	// Extract only filename from given path.
	char filename[50];
	int i = strlen(fpath);
	for(;i>0;i--)
		if(fpath[i-1]=='\\')
			break;

	for(int j=0;i<=(int)strlen(fpath);i++)
		filename[j++] = fpath[i];
	////////////////////////////////////////

	ifstream myFile (fpath, ios::in|ios::binary|ios::ate);
	int size = (int)myFile.tellg();
	myFile.close();

	char filesize[10];
	sprintf_s(filesize, 10, "%d", size);

	send( m_socket, filename, strlen(filename), 0 );
	char rec[32] = "";
	recv( m_socket, rec, 32, 0 );

	send( m_socket, filesize, strlen(filesize), 0 );
	recv( m_socket, rec, 32, 0 );

	FILE *fr;
	fopen_s( &fr, fpath, "rb" );

	while(size > 0)
	{
		char buffer[1030];

		if(size>=1024)
		{
			fread(buffer, 1024, 1, fr);
			send( m_socket, buffer, 1024, 0 );
			recv( m_socket, rec, 32, 0 );

		}
		else
		{
			fread(buffer, size, 1, fr);
			buffer[size]='\0';
			send( m_socket, buffer, size, 0 );
			recv( m_socket, rec, 32, 0 );
		}
		size -= 1024;
	}
	fclose(fr);
}
/**/