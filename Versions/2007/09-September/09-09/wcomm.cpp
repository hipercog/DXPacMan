#include "wcomm.h"

WSADATA wsaData;
SOCKET m_socket;
SOCKET m_backup;
sockaddr_in con;
SOCKET AcceptSocket;
bool open = false;

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

int WComm::connectServer(char *ip, int port)
{
    // Connect to a server.
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( ip );
    con.sin_port = htons( port );

    if ( connect( m_socket, (SOCKADDR*) &con, sizeof(con) ) == SOCKET_ERROR) {
        //printf( "Failed to connect.\n" );
        WSACleanup();
        return -1;
    }else
		open = true;
	
	return 0;
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
