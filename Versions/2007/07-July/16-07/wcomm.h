#include "winsock.h"

class WComm
{

public:
	
	WComm();
	~WComm();

	void connectServer(char*,int);
	 int sendData(const char*);
	 int recvData(char*,int);
//	void fileSend(char*);
//	void closeConnection();
};

/**/
//Server Connect Code - this can be used to link to a server which is listening for the output of this game - to be put in universals.cpp

/**/