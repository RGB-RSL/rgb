
#include "Logger.h"
#include <cassert>

#define __LOG true

using namespace std;

Logger::LogStreams *Logger::logStreams = NULL;

ostream &beginCycle(ostream &stream)
{
	stream << endl << "[--BEGINNING OF CYCLE--]" << endl;
	return stream;
}

ostream &endCycle(ostream &stream)
{
	stream << "[-----END OF CYCLE-----]" << endl << endl;
	return stream;
}

// class LogStream

LogStream::LogStream(ostream *stream, const Timer *timer): mStream(stream), mActive(true),
mTimer(timer), mFirst(true)
{
}

LogStream::~LogStream()
{
}

LogStream &LogStream::operator<<(ostream &(*man)(ostream &))
{
	if(mStream && mActive)
	{
		*mStream << man;
		if(man == (ostream & (*)(ostream &))endl)
			mFirst = true;
	}
	return *this;
}

LogStream &LogStream::operator<<(ios_base &(*man)(ios_base &))
{
	if(mStream && mActive)
		*mStream << man;
	return *this;
}

LogStream &LogStream::operator<<(LogStream &(*man)(LogStream &))
{
	return man(*this);
}

const Timer *LogStream::timer() const
{
	return mTimer;
}

void LogStream::setTimer(const Timer *timer)
{
	mTimer = timer;
}

void LogStream::suspend()
{
	if(mActive)
	{
		*this << "LOGGER: Suspended!" << endl;
		mActive = false;
	}
}

void LogStream::resume()
{
	if(!mActive)
	{
		mActive = true;
		*this << "LOGGER: Resumed!" << endl;
	}
}

// class LogFile

LogFile::LogFile(const string &fileName, const Timer *timer):
LogStream(__LOG ? new ofstream(fileName.c_str()) : NULL, __LOG ? timer : NULL),
mFileName(fileName)
{
}

LogFile::LogFile(const Timer *timer): LogStream(dynamic_cast<ostream*>(&mLocalStream), timer)
{
}

LogFile::~LogFile()
{
	delete mStream;
}

void LogFile::setFileName(const string &fileName)
{
#if __LOG
	mStream = __LOG ? new ofstream(fileName.c_str()) : NULL;
	*mStream << mLocalStream.str();
#endif
}

std::string LogFile::getFileName()
{
	return mFileName;
}

// class LogDevNull

LogDevNull::LogDevNull(): LogStream(NULL, NULL)
{
}

LogDevNull::~LogDevNull()
{
}


// class LogStdOut

LogStdOut::LogStdOut(const Timer *timer): LogStream(&cout, timer)
{
}

LogStdOut::~LogStdOut()
{
}


// class LogStdErr

LogStdErr::LogStdErr(const Timer *timer): LogStream(&cerr, timer)
{
}

LogStdErr::~LogStdErr()
{
}


// class Logger::LogStreams

Logger::LogStreams::LogStreams()
{
}

Logger::LogStreams::~LogStreams()
{
	for(map<string, LogStream *>::iterator it = streams.begin();
	  it != streams.end(); it++)
		delete it->second;
}

bool LogStream::getShowBeginEnd()
{
	return mShowBeginEnd;
}

void LogStream::setShowBeginEnd(bool showBeginEnd)
{
	mShowBeginEnd = showBeginEnd;
}

void Logger::LogStreams::add(const string &name, LogStream *stream,
							 bool showBeginEnd)
{
	map<string, LogStream *>::iterator it = streams.find(name);
	if(it != streams.end()) return;

	streams[name] = stream;
	streams[name]->setShowBeginEnd(showBeginEnd);
}

LogStream &Logger::LogStreams::operator[](const string &name)
{
	map<string, LogStream *>::const_iterator it = streams.find(name);
	if(it == streams.end())
		return (LogStream &)devNull;

	return *it->second;
}

std::map<std::string, LogStream *> &Logger::LogStreams::getStreams()
{
	return streams;
}

void Logger::LogStreams::showBeginCycle()
{
	for(map<string, LogStream *>::iterator it = streams.begin();
	  it != streams.end(); it++)
		if(it->second->getShowBeginEnd())
			*it->second << beginCycle;
}

void Logger::LogStreams::showEndCycle()
{
	for(map<string, LogStream *>::iterator it = streams.begin();
	  it != streams.end(); it++)
		if(it->second->getShowBeginEnd())
			*it->second << endCycle;
}


// class Logger

Logger::Logger()
{
	if(!logStreams)
		logStreams = new LogStreams();

	//	refCount++;
}

Logger::~Logger()
{
	//	if (!--refCount)
	//		delete logStreams;
}

void Logger::add(const string &name, LogStream *stream, bool showBeginEnd)
{
	assert(logStreams);
	logStreams->add(name, stream, showBeginEnd);
}

LogStream &Logger::operator[](const string &name)
{
	assert(logStreams);
	return (*logStreams)[name];
}

Logger::LogStreams &Logger::getLogStreams()
{
	return *logStreams;
}

void Logger::showBeginCycle()
{
	assert(logStreams);
	logStreams->showBeginCycle();
}

void Logger::showEndCycle()
{
	assert(logStreams);
	logStreams->showEndCycle();
}

