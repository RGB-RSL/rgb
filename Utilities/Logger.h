
#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include "Timer.h"
#include <sstream>

#define TIME_FIELD_WIDTH 4
#define TIME_FIELD_FILL '0'
#define TIME_FIELD_PRECISION 2

class LogStream
{
protected:
	std::ostream *mStream;
	bool mActive;
	const Timer *mTimer;
	bool mFirst;
	bool mShowBeginEnd;

public:
	LogStream(std::ostream *stream, const Timer *timer);
	virtual ~LogStream();

	bool getShowBeginEnd();
	void setShowBeginEnd(bool = true);

	template<class T>
	LogStream & operator<<(const T &value)
	{
		if(mStream && mActive)
		{
			std::ostringstream s;
			s << value;
			std::string ss(s.str());
			while(ss.find('\n') < ss.size())
			{
				if(mFirst && mShowBeginEnd)
					*mStream << "  " << std::setw(TIME_FIELD_WIDTH) << std::setfill(TIME_FIELD_FILL)
					<< (mTimer ? mTimer->getCycleTime() : 0) << ". ";
				*mStream << ss.substr(0, ss.find("\n") + 1);
				ss = ss.substr(ss.find("\n") + 1, ss.size() - ss.find("\n") - 1);
				mFirst = true;
			}
			if(mFirst && mShowBeginEnd)
				*mStream << "  " << std::setw(TIME_FIELD_WIDTH) << std::setfill(TIME_FIELD_FILL)
				<< (mTimer ? mTimer->getCycleTime() : 0) << ". ";
			*mStream << ss;
			mFirst = false;
		}
		return *this;
	}

	LogStream & operator<<(std::ostream &(*man)(std::ostream &));
	LogStream & operator<<(std::ios_base &(*man)(std::ios_base &));
	LogStream & operator<<(LogStream &(*man)(LogStream &));

	const Timer *timer() const;
	void setTimer(const Timer *timer);
	void suspend();
	void resume();
};

std::ostream &beginCycle(std::ostream &stream);
std::ostream &endCycle(std::ostream &stream);

class LogFile: public LogStream
{
protected:
	std::string mFileName;
	std::ostringstream mLocalStream;

public:
	LogFile(const std::string &fileName, const Timer *timer);
	LogFile(const Timer *timer);
	void setFileName(const std::string &fileName);
	std::string getFileName();
	virtual ~LogFile();
};

class LogDevNull: public LogStream
{
public:
	LogDevNull();
	virtual ~LogDevNull();
};

class LogStdOut: public LogStream
{
public:
	LogStdOut(const Timer *timer);
	virtual ~LogStdOut();
};

class LogStdErr: public LogStream
{
public:
	LogStdErr(const Timer *timer);
	virtual ~LogStdErr();
};

class Logger
{
public:

	class LogStreams
	{
	protected:
		std::map<std::string, LogStream *> streams;
		LogDevNull devNull;

	public:
		LogStreams();
		virtual ~LogStreams();

		void add(const std::string &name, LogStream *stream, bool = true);
		LogStream & operator[](const std::string &name);
		std::map<std::string, LogStream *> &getStreams();

		void showBeginCycle();
		void showEndCycle();
	};

protected:
	static LogStreams *logStreams;

public:
	Logger();
	virtual ~Logger();

	void add(const std::string &name, LogStream *stream, bool = true);
	LogStream & operator[](const std::string &name);
	LogStreams &getLogStreams();

	void showBeginCycle();
	void showEndCycle();
};

#endif // __LOGGER_H_
