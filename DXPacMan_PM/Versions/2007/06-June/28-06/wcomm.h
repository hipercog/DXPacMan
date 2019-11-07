#include "winsock2.h"
#include "main.h"

class WComm
{

public:
	
	WComm();
	~WComm();

	void connectServer(char*,int);
	 int sendData(char*);
	 int recvData(char*,int);
	void fileSend(char*);
	void closeConnection();
};

/** /
//Server Connect Code - this can be used to link to a server which is listening for the output of this game - to be put in main.cpp

void runclient()
{
	char rec[32] = "";

	// Connect To Server
	w.connectServer(ip_no#, port_no#);
	printf("Connected to server...\n");

	// Sending File
	w.sendData("FileSend");	w.recvData(rec,32);
	outputState(); //Set this to write the deque of states to the socket stream, not a local file
	printf("File Sent.............\n");

	// Send Close Connection Signal
	w.sendData("EndConnection");w.recvData(rec,32);
	printf("Connection ended......\n");
}
/**/