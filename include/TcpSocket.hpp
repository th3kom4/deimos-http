#pragma once

class ClientConnection;

class TcpSocket {
public:	
	TcpSocket(int portno);
	~TcpSocket();
	ClientConnection accept_connection();

private:
	int sock_fd;
};
