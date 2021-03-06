/*
 *  Copyright 2002-2005, Simorgh Team, Allameh Helli High School (NODET).
 *
 *  This program is free software, you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Library General Public License for more details.
 *
 *  This file is created by: Mohsen Izadi
 *  and is modified by: Ahmad Boorghany
 *
 *  Released on Monday 1 August 2005, 10 Mordad 1384 by Simorgh RoboCup Team.
 *  For more information please read README file.
 */

#include "Config.h"
#include <cassert>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "Basics.h"


#include <iostream>

using namespace std;
using namespace basics;

bool Config::isCI(false);

//Config config;
Config::ConfigFiles *Config::configFiles = NULL;

// class ConfigValue

ConfigValue::ConfigValue(const string &value): value(value)
{
	tokenizeValue();
}

void ConfigValue::tokenizeValue()
{
	signed pos = 0;
	//this warning can't be resolved (!)
	while(pos <= value.length())
	{
		signed epos = value.find_first_of(" \t", pos);
		if(epos == (int)string::npos) epos = value.length();
		values.push_back(Str(value.substr(pos, epos - pos)));
		pos = value.find_first_not_of(" \t", epos + 1);
	}
}

ConfigValue::~ConfigValue()
{
}

ostream & operator <<(ostream & _os, ConfigValue const &_configValue)
{
	return _os << _configValue;
}

int ConfigValue::size() const
{
	return values.size();
}

void ConfigValue::setString(std::string v)
{
	value = v;
	tokenizeValue();
}

void ConfigValue::setInt(int v)
{
	value = string(Str(v).as<string > ());
}

const Str &ConfigValue::operator[](int index)const
{
	return values[index];
}

const string &ConfigValue::asString() const
{
	return value;
}

char ConfigValue::asChar() const
{
	assert(values.size() > 0);
	return values[0].as<char>();
}

bool ConfigValue::asBool() const
{
	assert(values.size() > 0);
	return values[0].as<bool>();
}

int ConfigValue::asInt() const
{
	assert(values.size() > 0);
	return values[0].as<int>();
}

float ConfigValue::asFloat() const
{
	assert(values.size() > 0);
	return values[0].as<float>();
}


// class ConfigGroup

void ConfigGroup::add(const string &name, const string &value)
{
	map<string, ConfigValue>::iterator it = values.find(name);
	if(it != values.end())
	{
		it->second = ConfigValue(value);
	}
	else
	{
		values.insert(make_pair(name, ConfigValue(value)));
	}
}

const ConfigValue &ConfigGroup::operator[](const std::string &name) const
{
	map<string, ConfigValue>::const_iterator it = values.find(name);
	if(it == values.end())
	{
		//		cerr << name;
	}
	assert(it != values.end());
	return it->second;
}

bool ConfigGroup::exists(const std::string &name)
{
	map<string, ConfigValue>::iterator it = values.find(name);
	return (it != values.end());
}

ConfigValue &ConfigGroup::operator[](const std::string &name)
{
	map<string, ConfigValue>::iterator it = values.find(name);
	assert(it != values.end());
	return it->second;
}

// class ConfigFile

ConfigFile::ConfigFile()
{
}

ConfigFile::ConfigFile(const string &fileName)
{
	setByFile(fileName);
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::readFile(const string &fileName)
{
	ifstream fin(fileName.c_str());
	assert(fin);
	string line;
	addGroup("");
	map<string, ConfigGroup>::iterator currentGroupIt = groups.find("");
	while(getline(fin, line))
	{
		unsigned cpos = line.find_first_of('#');
		unsigned nb = line.find_first_not_of(" \t");
		unsigned ve = line.find_last_not_of(" \t", cpos ? cpos - 1 : 0);
		if(ve <= nb) continue;
		if(line[nb] == '[')
		{
			assert(line[ve] == ']');
			string group = line.substr(nb + 1, ve - nb - 1);
			addGroup(group);
			currentGroupIt = groups.find(group);
		}
		else
		{
			unsigned qpos = line.find_first_of('=');
			assert(qpos != string::npos && qpos < cpos);
			unsigned ne = line.find_last_not_of(" \t", qpos - 1);
			unsigned vb = line.find_first_not_of(" \t", qpos + 1);
			string name = line.substr(nb, ne - nb + 1);
			assert(name != "");
			string value = line.substr(vb, ve - vb + 1);

			currentGroupIt->second.add(name, value);
		}
	}
}

void ConfigFile::setByFile(const string &fileName)
{
	readFile(fileName);
}

bool ConfigFile::setByArguments(int argc, char **argv, ConfigFile logger)
{
	bool error = false;
	for(int i = 1; i < argc && !error; i++)
	{
		if(!strcmp(argv[i], "--goalie"))
			(*this)["Public"].add("IsGoalie", "True");
		else if(!strcmp(argv[i], "--offline"))
		{
			(*this)["Server"].add("OfflinePlaying", "True");
			(logger)["Logger"].add("Offline", "True");
		}
		else if(!strcmp(argv[i], "--coach"))
			;
		else if(!strcmp(argv[i], "--host"))
			if(++i == argc)
				error = true;
			else
				(*this)["Connection"].add("HostName", argv[i]);
		else if(!strcmp(argv[i], "--port"))
			if(++i == argc)
				error = true;
			else
				(*this)["Connection"].add("Port", argv[i]);
		else if(!strcmp(argv[i], "--coach-port"))
			if(++i == argc)
				error = true;
			else
				(*this)["Server"].add("CoachPort", argv[i]);
		else if(!strcmp(argv[i], "--number"))
			if(++i == argc)
				error = true;
			else
				(*this)["Public"].add("UniformNum", argv[i]);
		else if(!strcmp(argv[i], "--team"))
			if(++i == argc)
				error = true;
			else
				(*this)["Public"].add("TeamName", argv[i]);
		else if(!strcmp(argv[i], "--random-seed"))
			if(++i == argc)
				error = true;
			else
				(*this)["Public"].add("RandomSeed", argv[i]);
		else
			error = true;
	}
	if(error)
		argumentsError(argv[0]);
	return !error;
}

void ConfigFile::argumentsError(const string &binName)
{
	cout << "Usage : " << binName << " [Options]" << endl;
	cout << "Options :" << endl;
	cout << "  --coach                   : Init and run as coach" << endl;
	cout << "  --goalie                  : Init agent as a goalie" << endl;
	cout << "  --offline                 : Run as offline agent" << endl;
	cout << "  --team teamname           : Set teamname to connect to server" << endl;
	cout << "  --host hostname           : Set hostname to connect to server" << endl;
	cout << "  --player-port portnum     : Set player port number to connect to server" << endl;
	cout << "  --coach-port portnum      : Set coach port number to connect to server" << endl;
	cout << "  --number pnum             : Set player number" << endl;
	cout << endl;
}

void ConfigFile::addGroup(const string &group)
{
	if(groups.find(group) != groups.end()) return;

	groups[group] = ConfigGroup();
}

const ConfigGroup &ConfigFile::operator[](const string &name) const
{
	map<string, ConfigGroup>::const_iterator it = groups.find(name);
	assert(it != groups.end());

	return it->second;
}

ConfigGroup &ConfigFile::operator[](const string &name)
{
	map<string, ConfigGroup>::iterator it = groups.find(name);

	assert(it != groups.end());

	return it->second;
}

// class Config::ConfigFiles

Config::ConfigFiles::ConfigFiles()
{
}

Config::ConfigFiles::~ConfigFiles()
{
	for(map<string, ConfigFile *>::iterator it = files.begin();
	  it != files.end(); it++)
		delete it->second;
}

void Config::ConfigFiles::add(const string &name)
{
	map<string, ConfigFile *>::iterator it = files.find(name);
	if(it != files.end()) return;
	files[name] = new ConfigFile();
}

void Config::ConfigFiles::add(const string &name, const string &fileName)
{
	map<string, ConfigFile *>::iterator it = files.find(name);
	if(it != files.end())
		it->second->setByFile(fileName);
	else
		files[name] = new ConfigFile(fileName);
}

const ConfigFile &Config::ConfigFiles::operator[](const string &name) const
{
	map<string, ConfigFile *>::const_iterator it = files.find(name);
	assert(it != files.end());

	return *it->second;
}

ConfigFile &Config::ConfigFiles::operator[](const string &name)
{
	map<string, ConfigFile *>::iterator it = files.find(name);
	assert(it != files.end());

	return *it->second;
}


// class Config

Config::Config()
{
	if(!configFiles)
		configFiles = new ConfigFiles();
	if(!isCI)
	{
		isCI = true;
		string configTopic;
		string fileCounterValue;
	}
}

Config::~Config()
{
}

void Config::add(const std::string &name)
{
	assert(configFiles);
	configFiles->add(name);
}

void Config::add(const std::string &name, const std::string &fileName)
{
	assert(configFiles);
	configFiles->add(name, fileName);
}

const ConfigFile &Config::operator[](const string &name) const
{
	assert(configFiles);
	return (*configFiles)[name];
}

ConfigFile &Config::operator[](const string &name)
{
	assert(configFiles);
	return (*configFiles)[name];
}

//config["walk"].printToFile();

void ConfigFile::printToFile(const std::string &fileName)
{
	ofstream fin(fileName.c_str());
	assert(fin);
	for(map<string, ConfigGroup>::iterator i = groups.begin(); i != groups.end(); i++)
	{
		if(i->first == "")
			continue;
		fin << "[" << i->first << "]" << endl;
		for(map<string, ConfigValue>::iterator j = i->second.values.begin(); j != i->second.values.end(); j++)
		{
			fin << "\t\t" << j->first << " " << "=" << " ";
			for(vector<Str>::iterator l = j->second.values.begin(); l != j->second.values.end(); l++)
				fin << l->as<string > () << " ";
			fin << endl;
		}
		fin << endl;
	}
}
