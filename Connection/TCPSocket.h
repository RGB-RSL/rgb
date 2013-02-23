
#ifndef RCSS_NET_TCPSOCKET_H
#define RCSS_NET_TCPSOCKET_H

#include "Socket.h"

class TCPSocket: public Socket
{
public:
	TCPSocket();
	TCPSocket(const Addr& addr);
	TCPSocket(const Addr& addr, const Addr& dest);
	virtual Socket* accept(Addr& addr);

protected:
	TCPSocket(int socket);
	virtual void doOpen(int& fd);
};

#endif
