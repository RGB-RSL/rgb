
#include "Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include "Exception.h"

Socket::Socket(): m_socket(0), m_open(false), m_connected(false)
{
}

Socket::~Socket()
{
	close();
}

void Socket::open()
{
	doOpen(m_socket);
	m_open = true;
	int err = setCloseOnExec();
	if(err < 0)
	{
		err = errno;
		close();
		throw OpenErr(err);
	}
}

void Socket::bind(const Addr& addr)
{
	int err = ::bind(m_socket, (struct sockaddr *)&(addr.getAddr()), sizeof (addr.getAddr()));
	if(err < 0)
		throw BindErr(errno);
}

void Socket::listen(int backlog)
{
	int err = ::listen(m_socket, backlog);
	if(err < 0)
		throw ListenErr(errno);
}

Socket* Socket::accept(Addr& addr)
{
	throw AcceptErr(EOPNOTSUPP);
	return 0;
}

Addr Socket::getName() const
{
	Addr::AddrType name;
	socklen_t from_len = sizeof (name);
	int err = ::getsockname(m_socket, (struct sockaddr *) & name, &from_len);
	if(err < 0)
		throw GetNameErr(errno);

	return Addr(name);
}

void Socket::connect(const Addr& addr)
{
	int err = ::connect(m_socket, (const struct sockaddr *)&(addr.getAddr()), sizeof (addr.getAddr()));
	if(err < 0)
		throw ConnectErr(errno);

	m_connected = true;
}

Addr Socket::getPeer() const
{
	Addr::AddrType name;
	socklen_t from_len = sizeof (name);
	int err = ::getpeername(m_socket, (struct sockaddr *) & name, &from_len);
	if(err < 0)
		throw GetNameErr(errno);

	return Addr(name);
}

void Socket::close()
{
	if(m_open)
	{
		m_open = false;
		::close(m_socket);
		m_socket = 0;
	}

	m_connected = false;
}

int Socket::setCloseOnExec(bool on)
{
	return fcntl(m_socket, F_SETFD, (on ? FD_CLOEXEC : ~FD_CLOEXEC));
}

int Socket::setNonBlocking(bool on)
{
	int flags = fcntl(m_socket, F_GETFL, 0);
	if(flags == -1)
		return flags;

	if(on)
		return fcntl(m_socket, F_SETFL, O_NONBLOCK | flags);
	else
		return fcntl(m_socket, F_SETFL, ~O_NONBLOCK & flags);
}

int Socket::setAsync(bool on)
{
	errno = EPERM;
	return -1;
}

int Socket::setBroadcast(bool on)
{
	errno = EPERM;
	return -1;
}

int Socket::getFD() const
{
	return m_socket;
}

bool Socket::isOpen() const
{
	return m_open;
}

bool Socket::isConnected() const
{
	return m_connected;
}

Addr Socket::getDest() const
{
	return getPeer();
}

int Socket::send(const char* msg, size_t len, const Addr& dest, int flags, CheckingType check)
{
	if(check == DONT_CHECK)
	{
		return ::sendto(m_socket, msg, len, flags, (struct sockaddr *)&(dest.getAddr()), sizeof (dest.getAddr()));
	}
	else
	{
		for(;;)
		{
			int sent = ::sendto(m_socket, msg, len, flags, (struct sockaddr *)&(dest.getAddr()), sizeof (dest.getAddr()));
			if(sent != -1 || (errno != EINTR && errno != EWOULDBLOCK))
				return sent;
		}
	}
}

int Socket::send(const char* msg, size_t len, int flags, CheckingType check)
{
	if(check == DONT_CHECK)
	{
		return ::send(m_socket, msg, len, flags);
	}
	else
	{
		for(;;)
		{
			int sent = ::send(m_socket, msg, len, flags);
			if(sent != -1 || (errno != EINTR && errno != EWOULDBLOCK))
				return sent;
		}
	}
}

int Socket::recv(char* msg, size_t len, Addr& from, int flags, CheckingType check)
{
	if(check == DONT_CHECK)
	{
		Addr::AddrType addr;
		socklen_t from_len = sizeof (addr);
		int rval = ::recvfrom(m_socket, msg, len, flags, (struct sockaddr *) & addr, &from_len);
		from = Addr(addr);
		return rval;
	}
	else
	{
		for(;;)
		{
			Addr::AddrType addr;
			socklen_t from_len = sizeof (addr);
			int received = ::recvfrom(m_socket, msg, len, flags, (struct sockaddr *) & addr, &from_len);
			from = Addr(addr);
			if(received != -1 || errno != EINTR)
				return received;
		}
	}
}

int Socket::recv(char* msg, size_t len, int flags, CheckingType check)
{
	if(check == DONT_CHECK)
		return ::recv(m_socket, msg, len, flags);
	else
	{
		for(;;)
		{
			int received = ::recv(m_socket, msg, len, flags);
			if(received != -1 || errno != EINTR)
				return received;
		}
	}
}

int Socket::recv(int timeout, char* msg, size_t len, Addr& from, int flags)
{
	errno = EPERM;
	return -1;
}

int Socket::recv(int timeout, char* msg, size_t len, int flags)
{
	errno = EPERM;
	return -1;
}
