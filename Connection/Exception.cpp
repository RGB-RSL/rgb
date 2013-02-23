
#include "Exception.h"
#include <cstring>
#include <sys/types.h>
#include <netinet/in.h>

HostNotFound::HostNotFound(int err) throw (): m_err(err)
{
}

const char* HostNotFound::what() const throw ()
{
	return "Network error";
}

int HostNotFound::err() const throw ()
{
	return m_err;
}

OpenErr::OpenErr(int err) throw (): m_err(err)
{
}

const char* OpenErr::what() const throw ()
{
	return std::strerror(m_err);
}

int OpenErr::err() const throw ()
{
	return m_err;
}

BindErr::BindErr(int err) throw (): m_err(err)
{
}

const char* BindErr::what() const throw ()
{
	return strerror(m_err);
}

int BindErr::err() const throw ()
{
	return m_err;
}

ListenErr::ListenErr(int err) throw (): m_err(err)
{
}

const char* ListenErr::what() const throw ()
{
	return strerror(m_err);
}

int ListenErr::err() const throw ()
{
	return m_err;
}

AcceptErr::AcceptErr(int err) throw (): m_err(err)
{
}

const char* AcceptErr::what() const throw ()
{
	return strerror(m_err);
}

int AcceptErr::err() const throw ()
{
	return m_err;
}

GetNameErr::GetNameErr(int err) throw (): m_err(err)
{
}

const char* GetNameErr::what() const throw ()
{
	return strerror(m_err);
}

int GetNameErr::err() const throw ()
{
	return m_err;
}

ConnectErr::ConnectErr(int err) throw (): m_err(err)
{
}

const char* ConnectErr::what() const throw ()
{
	return strerror(m_err);
}

int ConnectErr::err() const throw ()
{
	return m_err;
}
