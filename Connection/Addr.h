
#ifndef RCSS_NET_ADDR_H
#define RCSS_NET_ADDR_H

struct sockaddr_in;

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

class AddrImpl;

class Addr
{
public:
	typedef boost::uint16_t PortType;
	typedef boost::uint32_t HostType;
	typedef struct sockaddr_in AddrType;

	static const HostType BROADCAST;
	static const HostType ANY;

	Addr();
	Addr(const AddrType& addr);
	Addr(PortType port);
	Addr(PortType port, HostType host);
	Addr(PortType port, const std::string& host);

	const AddrType& getAddr() const;
	PortType getPort() const;

	void setPort(PortType port);

	HostType getHost() const;

	std::string getHostStr() const;

	bool operator ==(const Addr& addr) const;
	bool operator<(const Addr& addr) const;

private:
	boost::shared_ptr< AddrImpl > m_impl;
};

std::ostream &
operator<<(std::ostream& o, const Addr& addr);

#endif
