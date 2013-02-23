
#ifndef RCSS_NET_EXCPETION_H
#define RCSS_NET_EXCEPTION_H

#include <exception>

class HostNotFound: public std::exception
{
public:
	HostNotFound(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class OpenErr: public std::exception
{
public:
	OpenErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class BindErr: public std::exception
{
public:
	BindErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class ListenErr: public std::exception
{
public:
	ListenErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class AcceptErr: public std::exception
{
public:
	AcceptErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class GetNameErr: public std::exception
{
public:
	GetNameErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

class ConnectErr: public std::exception
{
public:
	ConnectErr(int err) throw ();

	const char* what() const throw ();

	int err() const throw ();

private:
	int m_err;
};

#endif
