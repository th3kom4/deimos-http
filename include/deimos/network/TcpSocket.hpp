#pragma once

class ClientConnection;

class TcpSocket {
public:	
	TcpSocket(int portno);
	~TcpSocket();

	TcpSocket(const TcpSocket&) = delete;
	TcpSocket& operator=(const TcpSocket&) = delete;

	ClientConnection* accept_connection();

	int get_fd() const { return sock_fd; }

private:
	int sock_fd;
	void make_non_blocking(int fd);
};
