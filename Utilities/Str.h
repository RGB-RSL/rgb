
#ifndef __STR_H_
#define __STR_H_

#include <string>
#include <vector>
#include "Debugger.h"
#include "Types.h"

class Str
{
protected:
	std::string stringVal;

public:

	template <class _T>
	Str(const _T &b_value)
	{
		std::ostringstream ss;
		ss << b_value;
		stringVal = ss.str();
	}

	template <class _T, class _U>
	Str(const std::pair<_T, _U> &b_value)
	{
		std::ostringstream ss;
		ss << b_value.first << "," << b_value.second;
		stringVal = ss.str();
	}

	template <class _T>
	Str(const std::vector<_T> &b_value)
	{
		std::ostringstream ss;
		for(unsigned it = 0; it < b_value.size(); it++)
			ss << b_value[it] << "|";
		stringVal = ss.str();
	}

	template <class _T, class _U>
	Str(const std::map<_T, _U> &b_value)
	{
		std::ostringstream ss;
		for(unsigned it = 0; it < b_value.size(); it++)
			ss << b_value[it].first << ", " << b_value[it].second << "| ";
		stringVal = ss.str();
	}

	template<typename _Tp, typename _T, typename _U>
	operator _Tp() const
	{
		if(Types::sameType<_Tp, int>() ||
		  Types::sameType < _Tp, bool>() ||
		  Types::sameType<_Tp, char>() ||
		  Types::sameType<_Tp, unsigned>() ||
		  Types::sameType<_Tp, float>() ||
		  Types::sameType<_Tp, double>() ||
		  Types::sameType<_Tp, std::string > () ||
		  Types::sameType<_Tp, std::pair<_T, _U> >() ||
		  Types::sameType<_Tp, std::vector<_T> >() ||
		  Types::sameType<_Tp, std::map<_T, _U> >())
			return this->as<_Tp > ();
		else
			return _Tp();
	}

	template <class _T>
	_T as() const
	{
		_T s = _T();
		return b_value(s);
	}

	template <class _T, class _U>
	std::pair<_T, _U> b_value(std::pair<_T, _U>) const
	{
		if(stringVal.find(",") != std::string::npos)
			return std::make_pair(Str(stringVal.substr(0, stringVal.find(","))).as<_T > (),
								Str(stringVal.substr(stringVal.find(",") + 1, stringVal.size() -
													 stringVal.find(",") - 1)).as<_U > ());
		else
			ASSERT(MAIN, false);
		_T s1 = _T();
		_U s2 = _U();
		return std::make_pair(s1, s2);
	}

	template <class _T>
	std::vector<_T> b_value(std::vector<_T>) const
	{
		std::vector<_T> vectorVal;
		std::string ss(stringVal);
		if(stringVal.find("|") < stringVal.size())
			while(ss.size() > 0)
			{
				vectorVal.push_back(Str(ss.substr(0, ss.find("|"))).as<_T > ());
				ss = ss.substr(ss.find("|") + 1, ss.size() - ss.find("|") - 1);
			}
		return vectorVal;
	}

	template <class _T, class _U>
	std::map<_T, _U> b_value(std::map<_T, _U>) const
	{
		std::map<_T, _U> mapVal;
		std::string ss(stringVal);
		if(stringVal.find("|") < stringVal.size())
			while(ss.size() > 0)
			{
				std::pair<_T, _U> s =
				  Str(ss.substr(0, ss.find("|"))).as<std::pair<_T, _U> >();
				mapVal[s.first] = s.second;
				ss = ss.substr(ss.find("|") + 1, ss.size() - ss.find("|") - 1);
			}
		return mapVal;
	}

	std::string b_value(std::string) const;
	char b_value(char) const;
	bool b_value(bool) const;
	int b_value(int) const;
	unsigned int b_value(unsigned int) const;
	float b_value(float) const;
	double b_value(double) const;
};

class StrConfig
{
protected:
	std::string value;

public:
	StrConfig(const std::string &value);
	virtual ~StrConfig();

	const std::string &asString() const;
	char asChar() const;
	bool asBool() const;
	int asInt() const;
	float asFloat() const;
};

#endif // __STR_H_
