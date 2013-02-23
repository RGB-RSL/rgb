/*
 * BoolStream.h
 *
 *  Created on: Jan 31, 2010
 *      Author: Hamed
 */

#include <vector>
#include <string>
#include "bits.h"
#include "Types/Types.h"
#ifndef BOOLSTREAM_H_
#define BOOLSTREAM_H_

class BoolStream
{
public:
	BoolStream();
	BoolStream(int);
	virtual ~BoolStream();

	void setSize(int); // in Bytes
	void setTail(int);
	void setHead(int);

	void clear();

	//BoolStream & operator << (bool);
	//BoolStream & operator << (int);
	//BoolStream & operator << (bits);

	void push_back(bool);
	void push_back(int);
	void push_back(bits);
	void push_back(char);
	void push_back(std::string);

	bool pop_bool();
	bits pop_bits(int);
	int pop_int();
	char pop_char();
	std::string pop_string(int);

	std::string getVal();
	std::string getVal(std::string);
	std::vector<byte> getValue();

	void setValue(std::vector<byte>);

private:
	int head, tail;
	std::vector <byte> value;
};

//#include "BoolStream.cpp"

#endif
