
#include "Str.h"

#include <cassert>
#include <cctype>
#include <sstream>
#include <algorithm>

using namespace std;

string Str::b_value(string) const
{
	return stringVal;
}

char Str::b_value(char) const
{
	return stringVal[0];
}

bool Str::b_value(bool) const
{
	string tmp;
	tmp.reserve(stringVal.length());
	for(string::const_iterator it = stringVal.begin(); it != stringVal.end(); it++)
		tmp += tolower(*it);
	return tmp == "true" || tmp == "1";
}

int Str::b_value(int) const
{
	char *end;
	int val = strtol(stringVal.c_str(), &end, 10);
	return val;
}

unsigned int Str::b_value(unsigned int) const
{
	char *end;
	unsigned val = (unsigned)strtol(stringVal.c_str(), &end, 10);
	return val;
}

float Str::b_value(float) const
{
	char *end;
	float val = strtof(stringVal.c_str(), &end);
	if(stringVal == "nan")
		val = .0f;
	return val;
}

double Str::b_value(double) const
{
	char *end;
	double val = strtod(stringVal.c_str(), &end);
	return val;
}

StrConfig::StrConfig(const string &value): value(value)
{
}

StrConfig::~StrConfig()
{
}

const string &StrConfig::asString() const
{
	return value;
}

char StrConfig::asChar() const
{
	assert(value.length() == 1);
	return value[0];
}

bool StrConfig::asBool() const
{
	string tmp = "";
	for(string::const_iterator it = value.begin(); it != value.end(); it++)
		tmp += tolower(*it);
	assert(tmp == "true" || tmp == "1" || tmp == "false" || tmp == "0");
	if(tmp == "true" || tmp == "1")
		return true;
	return false;
}

int StrConfig::asInt() const
{
	char *end;
	long val = strtol(value.c_str(), &end, 0);
	assert(end != value.c_str());
	return val;
}

float StrConfig::asFloat() const
{
	char *end;
	float val = strtof(value.c_str(), &end);
	assert(end != value.c_str());
	return val;
}
