/* 
 * File:   OfflineConnection.cpp
 * Author: mehran
 * 
 * Created on September 28, 2010, 2:21 PM
 */

#include "OfflineConnection.h"
#include <fstream>
using namespace std;

OfflineConnection::OfflineConnection(string fileDir)
{
	//    in = new ifstream(fileDir.c_str());
	in.open(fileDir.c_str(), ifstream::in);
	//    cout<<"offline file dir: " << fileDir << endl;
}

bool OfflineConnection::init()
{
	return true;
}

OfflineConnection::~OfflineConnection()
{
	in.close();
}

bool OfflineConnection::getMessage(std::vector<byte>& msg)
{
	if(in.eof())
		return false;
	unsigned int size = 0;
	msg.clear();
	in >> size;
	char *message = new char[size];
	in.read(message, size);
	msg.assign(message, message + size);
	return true;
}

bool OfflineConnection::sendMessage(const std::vector<byte>& msg)
{
	return true;
}

void OfflineConnection::done()
{
	in.close();
}