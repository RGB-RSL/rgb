/*
 * BoolStream.cpp
 *
 *  Created on: Jan 31, 2010
 *      Author: Hamed
 */

#include "BoolStream.h"
#include <iostream>
#include <string>

using namespace std;

BoolStream::BoolStream()
{
}

BoolStream::BoolStream(int size)
{
	head = tail = 0;
	value.resize(size);
}

BoolStream::~BoolStream()
{
	value.resize(0);
}

void BoolStream::setSize(int size)
{
	value.resize(size);
}

void BoolStream::setTail(int t)
{
	tail = t;
}

void BoolStream::setHead(int h)
{
	head = h;
}

void BoolStream::push_back(int a)
{
	push_back(bits(a, 32));
}

void BoolStream::push_back(bits a)
{
	for(int i = 0; i < a.getL(); i++)
		push_back((bool)(a.getA() & (1 << i)));
}

void BoolStream::push_back(bool a)
{
	if(a)
		value[tail / 8] |= (1 << (tail % 8));
	tail++;
}

void BoolStream::push_back(char a)
{
	push_back(bits(a, 8));
}

void BoolStream::push_back(string a)
{
	for(int i = 0; i < a.size(); i++)
		push_back(a[i]);
}

bool BoolStream::pop_bool()
{
	int val = value[head / 8] & (1 << (head % 8));
	head++;
	return val;
}

bits BoolStream::pop_bits(int l)
{
	int val = 0;
	for(int i = 0; i < l; i++)
		val |= (pop_bool() << i);

	return bits(val, l);
}

int BoolStream::pop_int()
{
	return pop_bits(32).getA();
}

char BoolStream::pop_char()
{
	return char (pop_bits(8).getA());
}

string BoolStream::pop_string(int l)
{
	string s = "";
	for(int i = 0; i < l; i++)
		s += string(1, pop_char());

	return s;
}

string BoolStream::getVal()
{
	string s;

	for(int i = 0; i < tail / 8; i++)
		for(int j = 0; j < 8 && 8 * i + j < tail; j++)
			s = s + ((value[i] & (1 << j)) ? "0" : "1");
}

string BoolStream::getVal(string sep)
{
	string s;

	//	cout << tail << endl;
	for(int i = 0; i <= tail / 8; i++)
	{
		for(int j = 0; j < 8 && 8 * i + j < tail; j++)
			s = s + ((value[i] & (1 << j)) ? "1" : "0");
		s = s + sep;
	}

	return s;
}

vector<byte> BoolStream::getValue()
{
	return this -> value;
}

void BoolStream::setValue(std::vector<byte> val)
{
	head = 0;
	tail = val.size() * 8;
	this -> value = val;
}

void BoolStream::clear()
{
	head = tail = 0;
	this -> value.clear();
}