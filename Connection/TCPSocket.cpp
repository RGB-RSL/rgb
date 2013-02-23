
#include "TCPSocket.h"
#include "Exception.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>

TCPSocket::TCPSocket()
{
	open();
}

TCPSocket::TCPSocket(const Addr& addr)
{
	open();
	bind(addr);
}

TCPSocket::TCPSocket(const Addr& addr, const Addr& dest)
{
	open();
	bind(addr);
	connect(dest);
}

TCPSocket::TCPSocket(int socket)
{
	m_open = true;
	m_connected = true;
	m_socket = socket;
}

void TCPSocket::doOpen(int& fd)
{
	close();
	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
		throw OpenErr(errno);
}

Socket* TCPSocket::accept(Addr& addr)
{
	socklen_t len = sizeof (struct sockaddr);
	int fd = ::accept(m_socket, (struct sockaddr *)&(addr.getAddr()), &len);

	if(fd < 0)
	{
		throw AcceptErr(errno);
	}

	return new TCPSocket(fd);
}
