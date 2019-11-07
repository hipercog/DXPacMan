// Header for Windows communications class

class WComm
{

private:
	WSADATA wsaData;
	SOCKET m_socket;
	SOCKET m_backup;
	sockaddr_in con;
	SOCKET AcceptSocket;

public:
	char* cliIP;

	WComm();
	~WComm();

	void connectServer(char*,int);
	int sendData(char*);
	int recvData(char*,int);
	void fileReceive(char*);
	void startServer(int);
	void waitForClient();
	char* returnClient();
	void closeConnection();

	void operator=(WComm arg);
};
//End code - add one more empty line:
