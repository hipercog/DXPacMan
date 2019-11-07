#include "stdafx.h"
#include "wcomm.h"

WComm::WComm()
{
    // Initialize Winsock.
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if ( iResult != NO_ERROR )
        printf("Error at WSAStartup()\n");

    // Create a socket.
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_socket == INVALID_SOCKET ) {
        printf( "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return;
    }

	m_backup = m_socket;
}

WComm::~WComm(){WSACleanup();}

void WComm::connectServer(char *ip,int port)
{
    // Connect to a server.
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( ip );
    con.sin_port = htons( port );

    if ( connect( m_socket, (SOCKADDR*) &con, sizeof(con) ) == SOCKET_ERROR) {
        printf( "Failed to connect.\n" );
        WSACleanup();
        return;
    }
}


void WComm::startServer(int port)
{
    // Connect to a server.
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    con.sin_port = htons( port );

    if ( bind( m_socket, (SOCKADDR*) &con, sizeof(con) ) == SOCKET_ERROR) {
        printf( "Failed to connect.\n" );
        WSACleanup();
        return;
    }

    // Listen on the socket.
    if ( listen( m_socket, 1 ) == SOCKET_ERROR )
        printf( "Error listening on socket.\n");
}

char* WComm::returnClient()
{
	AcceptSocket = INVALID_SOCKET;
	sockaddr_in from;
	int fromlen = sizeof(from);
    while ( AcceptSocket == INVALID_SOCKET ) {
        AcceptSocket = accept( m_backup, (struct sockaddr*)&from, &fromlen );
    }
    m_socket = AcceptSocket;

	char* clip = inet_ntoa(from.sin_addr);
	for(int i = 0; i < fromlen; i++)
		if(clip[i] == '.')
			clip[i] = '_';
	
	return clip;
}

void WComm::waitForClient()
{
		AcceptSocket = SOCKET_ERROR;
        while ( AcceptSocket == SOCKET_ERROR ) {
            AcceptSocket = accept( m_backup, NULL, NULL );
        }
        m_socket = AcceptSocket;
}

int WComm::sendData(char *sendbuf)
{
	return send( m_socket, sendbuf, strlen(sendbuf), 0 );
}


int WComm::recvData(char *recvbuf,int size)
{
	int sz = recv( m_socket, recvbuf, size, 0 );
	recvbuf[sz] = '\0';
	return sz;
}


void WComm::closeConnection()
{
	closesocket(m_socket);
	m_socket = m_backup;
}


void WComm::fileReceive(char *filename)
{
	char rec[32] = "";	
			
	recv( m_socket, filename, 32, 0 );
	send( m_socket, "OK", strlen("OK"), 0 );

	FILE *fw;
	fopen_s( &fw, filename, "wb");

	int recs = recv( m_socket, rec, 32, 0 );
	send( m_socket, "OK", strlen("OK"), 0 );

	rec[recs]='\0';
	int size = atoi(rec);			

	while(size > 0)
	{
		char buffer[1030];

		if(size>=1024)
		{
			recv( m_socket, buffer, 1024, 0 );
			send( m_socket, "OK", strlen("OK"), 0 );
			fwrite(buffer, 1024, 1, fw);

		}
		else
		{
			recv( m_socket, buffer, size, 0 );
			send( m_socket, "OK", strlen("OK"), 0 );
			buffer[size]='\0';
			fwrite(buffer, size, 1, fw);
		}
		size -= 1024;
	}
	fclose(fw);
}

void WComm::operator=(WComm arg)
{
	cliIP = arg.cliIP;
	wsaData = arg.wsaData;
	m_socket = arg.m_socket;
	m_backup = arg.m_backup;
	con = arg.con;
	AcceptSocket = arg.AcceptSocket;
}

//End code - add one more empty line:
