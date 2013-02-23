
#ifndef __DEBUGGER_H_
#define __DEBUGGER_H_

#include <string>
#include "Logger.h"

#define MSG(log, msg) Debugger::message(#log, __FILE__, __LINE__, #msg)
#define VALUE(log, expr) Debugger::value(#log, __FILE__, __LINE__, #expr, expr)
#define DO(log, command) { Debugger::doo(#log, __FILE__, __LINE__, #command); command; }
#define DONE(log, command) { command; Debugger::done(#log, __FILE__, __LINE__, #command); }
#define DONEDO(log, command) { Debugger::doo(#log, __FILE__, __LINE__, #command); command; \
		Debugger::done(#log, __FILE__, __LINE__, #command); }
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(log, expr) Debugger::assrt(#log, __FILE__, __LINE__, #expr, expr)
#define MARK(log) Debugger::mark(#log, __FILE__, __LINE__)
//#define LOG(log, loglevel) if (LOGLEVEL == loglevel) Debugger::logg(world->selfIDString)
#define LOG(log, loglevel) if (LOGLEVEL == loglevel) Debugger::logg(#log)
#define LOGL(log) Debugger::logg(#log)
#define OFFLOG(log) Debugger::logg(#log "Off")
#define LOGS Debugger::logStreams()
#define BEGIN_FUNC(class, name) LOG(Main, 3) << #class << ":: " << #name << "(): <begin>" << endl; try{
#define END_FUNC(class, name) }catch(...) {\
		LOG(Main, 3) << #class<<":: " << #name << "(): <end>" << endl; throw;}\
		LOG(Main, 1) << #class << ":: " << #name << "(): <end>" << endl;

class Debugger
{
private:
	static Logger logger;

public:
	static void message(const std::string &log, const std::string &file, int line,
						const std::string &message);

	template<class T>
	static void value(const std::string &log, const std::string &file, int line,
					const std::string &expression, const T& value)
	{
		logger[log] << file << ":" << line << " '" << expression << "' = "
		  << value << std::endl;
		std::cout << file << ":" << line << " '" << expression << "' = "
		  << value << std::endl;
	}
	static void doo(const std::string &log, const std::string &file, int line,
					const std::string &command);
	static void done(const std::string &log, const std::string &file, int line,
					const std::string &command);
	static void assrt(const std::string &log, const std::string &file, int line,
					const std::string &expression, bool value);
	static void mark(const std::string &log, const std::string &file, int line);
	static LogStream &logg(const std::string &log);
	static Logger::LogStreams &logStreams();
};

#endif // __DEBUGGER_H_
