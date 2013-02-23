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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <map>
#include <string>
#include <vector>
#include "Str.h"

class ConfigValue
{
	//protected:
public:
	std::string value;
	std::vector<Str> values;

public:
	ConfigValue(const std::string &value);
	virtual ~ConfigValue();

	friend std::ostream & operator<<(std::ostream &, ConfigValue const &);
	int size() const;

	void setString(std::string v);
	void setInt(int v);

	const Str & operator[](int index)const;

	const std::string &asString() const;
	char asChar() const;
	bool asBool() const;
	int asInt() const;
	float asFloat() const;
private:
	void tokenizeValue();
};

class ConfigGroup
{
	//protected:
public:
	std::map<std::string, ConfigValue> values;

public:
	void add(const std::string &name, const std::string &value);
	const ConfigValue & operator[](const std::string &name) const;
	ConfigValue & operator[](const std::string &name);
	bool exists(const std::string &name);
};

class ConfigFile
{
protected:
	std::map<std::string, ConfigGroup> groups;
protected:
	virtual void readFile(const std::string &fileName);

public:
	ConfigFile();
	ConfigFile(const std::string &fileName);
	virtual ~ConfigFile();

	void setByFile(const std::string &fileName);
	bool setByArguments(int argc, char **argv, ConfigFile);
	void argumentsError(const std::string &binName);

	void addGroup(const std::string &group);
	const ConfigGroup & operator[](const std::string &name) const;
	ConfigGroup & operator[](const std::string &name);
	void printToFile(const std::string &fileName);
};

class Config
{
protected:
	static bool isCI;

	class ConfigFiles
	{
	protected:
		std::map<std::string, ConfigFile *> files;

	public:
		ConfigFiles();
		virtual ~ConfigFiles();

		void add(const std::string &name);
		void add(const std::string &name, const std::string &fileName);
		const ConfigFile & operator[](const std::string &name) const;
		ConfigFile & operator[](const std::string &name);

	};

	static ConfigFiles *configFiles;

public:
	Config();
	virtual ~Config();

	void add(const std::string &name);
	void add(const std::string &name, const std::string &FileName);
	const ConfigFile & operator[](const std::string &name) const;

	//void Load() const;

	ConfigFile & operator[](const std::string &name);

};

extern Config config;
typedef ConfigGroup Param;

#endif // __CONFIG_H
