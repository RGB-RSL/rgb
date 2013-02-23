
#include "Debugger.h"
#include <cstdlib>
#include <cassert>

using namespace std;

Logger Debugger::logger;

void Debugger::message(const string &log, const string &file, int line, const string &message)
{
	logger[log] << file << ":" << line << " '" << message << "'" << endl;
}

void Debugger::doo(const string &log, const string &file, int line, const string &command)
{
	logger[log] << file << ":" << line << " DOING '" << command << "'" << endl;
}

void Debugger::done(const string &log, const string &file, int line, const string &command)
{
	logger[log] << file << ":" << line << " '" << command << "' DONE" << endl;
}

void Debugger::assrt(const string &log, const string &file, int line,
					 const string &expression, bool value)
{
	if(!value)
	{
		logger[log] << file << ":" << line << " '" << expression << "' FAILED!" << endl;
		cout << file << ":" << line << " '" << expression << "' FAILED!" << endl;
		assert(0);
	}
}

void Debugger::mark(const string &log, const string &file, int line)
{
	logger[log] << file << ":" << line << " <<< MARK >>>" << endl;
}

LogStream &Debugger::logg(const std::string &log)
{
	return logger[log];
}

Logger::LogStreams &Debugger::logStreams()
{
	return logger.getLogStreams();
}
