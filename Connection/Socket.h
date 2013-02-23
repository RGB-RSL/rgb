
#ifndef RCSS_NET_SOCKET_H
#define RCSS_NET_SOCKET_H

#include <boost/cstdint.hpp>
#include "Addr.h"

class Socket
{
public:

	enum CheckingType
	{
		CHECK,
		DONT_CHECK
	};

	Socket();
	virtual ~Socket();
	void open();
	void bind(const Addr& addr);
	void listen(int backlog);
	virtual Socket* accept(Addr& addr);
	Addr getName() const;
	void connect(const Addr& addr);
	Addr getPeer() const;
	void close();
	int setCloseOnExec(bool on = true);
	int setNonBlocking(bool on = true);
	int setAsync(bool on = true);
	int setBroadcast(bool on = true);
	int getFD() const;
	bool isOpen() const;
	bool isConnected() const;
	Addr getDest() const;
	int send(const char* msg, size_t len, const Addr& dest, int flags = 0, CheckingType check = CHECK);
	int send(const char* msg, size_t len, int flags = 0, CheckingType check = CHECK);
	int recv(char* msg, size_t len, Addr& from, int flags = 0, CheckingType check = CHECK);
	int recv(char* msg, size_t len, int flags = 0, CheckingType check = CHECK);
	int recv(int timeout, char* msg, size_t len, Addr& from, int flags = 0);
	int recv(int timeout, char* msg, size_t len, int flags = 0);

protected:
	virtual void doOpen(int& fd) = 0;

	Socket(const Socket&);
	Socket & operator=(const Socket&);

protected:
	int m_socket;
	bool m_open;
	bool m_connected;
};

#endif
