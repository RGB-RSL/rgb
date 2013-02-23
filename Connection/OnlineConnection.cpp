#include "OnlineConnection.h"
#include "Exception.h"
//#include <iostream>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <vector>
//#include "../Base/Translator/Parser/Parser.h"
//#include <Config.h>
//#include <Basics.h>
#include "../Utilities/Debugger.h"

#define LOGLEVEL 0
using namespace std;
//using namespace basics;

OnlineConnection::OnlineConnection(const std::string& host, unsigned port): mHost(host), mPort(port), isDelete(false)
{
}

OnlineConnection::~OnlineConnection()
{
	mSocket.close();
	cout << "Closing Connection to " << mHost << ":" << mPort << "\n";
}

/*TCPSocket Connection::getSocket()
{
	return mSocket;
}
 */

/*Connection * Connection::instance()
{
	static Connection * mConnection = 0;
	if (!mConnection )//&& !config["ServerInitialize"]["Server"]["OfflinePlaying"].asBool())
	{
		mConnection = new Connection("localhost",7000);
		//config["ServerInitialize"]["Connection"]["HostName"].asString(), config["ServerInitialize"]["Connection"]["Port"].asInt());
		//LOG(Main , 1) << "if " << endl;
	}
	return mConnection;
}*/

bool OnlineConnection::selectInput()
{
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(mSocket.getFD(), &readfds);
	return select(mSocket.getFD() + 1, &readfds, 0, 0, 0) > 0;
}

bool OnlineConnection::init()
{
	cerr << "Connecting to TCP " << mHost << ":" << mPort << endl;
	try
	{
		Addr local(INADDR_ANY, INADDR_ANY);
		mSocket.bind(local);
	} catch(BindErr error)
	{
		cerr << "Failed to bind socket with '" << error.what() << "'" << endl;

		mSocket.close();
		return false;
	}
	try
	{
		Addr server(mPort, mHost);
		mSocket.connect(server);
	} catch(ConnectErr error)
	{
		cerr << "Connection failed with: '" << error.what() << "'" << endl;
		mSocket.close();
		return false;
	}
	return true;
}

bool OnlineConnection::sendMessage(const vector<unsigned char> &msg)
{
	int msgLen = msg.size();
#if LOGLEVEL > 0
	cout << "Connection->Send:";
	for(int i = 0; i < msgLen; i++)
		cout << " " << "" << (int)msg[i];
	cout << endl;
#endif
	size_t si = write(mSocket.getFD(), &(msg[0]), msgLen);
	if(si == -1)
	{
		cerr
		  << "(PutMessage) ERROR: "
		  << " send returned error '" << endl;
		return false;
	}
	return true;
}

bool OnlineConnection::getMessage(vector<byte> &msg)
{
	//	cout << "@I am in get message" << endl;

	if(!selectInput())
	{

		return false;
	}

	//	cout << "@first if passed" << endl;

	msg.clear();

	unsigned totalRead;
	unsigned expectedRead;
	byte *offset;
	byte *start;
	int bytesRead;


	unsigned int msgLen = 0;
	offset = (byte*) & msgLen;
	start = offset;
	expectedRead = sizeof (unsigned int);
	totalRead = 0;

	while(totalRead < expectedRead)
	{
		bytesRead = -1;
		while(bytesRead < 0)
		{
			bytesRead = read(mSocket.getFD(), offset, expectedRead - totalRead);
		}
		if(bytesRead == 0)
			return false;
		totalRead += bytesRead;
		offset = start + totalRead;
	}
	msgLen = ntohl(msgLen);
#if LOGLEVEL > 0
	cout << "Connection->Recv, Len:" << msgLen << endl;
#endif
	msg.resize(msgLen);
	//*((int*)msg[0]) = htonl(msgLen);
	offset = &(msg[0]);
	start = offset;
	expectedRead = msgLen;
	totalRead = 0;

	while(totalRead < expectedRead)
	{
		bytesRead = -1;
		while(bytesRead < 0)
		{
			bytesRead = read(mSocket.getFD(), offset, expectedRead - totalRead);
		}
		if(bytesRead == 0)
			return false;
		totalRead += bytesRead;
		offset = start + totalRead;
	}

#if LOGLEVEL > 0
	cout << "Connection->Recv, Msg: ";
	for(unsigned int i = 0; i < msgLen && i < 1000; i++)
		cout << (int)start[i] << " ";
	cout << endl;
	cout.flush();
#endif
	return true;

}

void OnlineConnection::done()
{
	mSocket.close();
	cerr << "closed connection to " << mHost << ":" << mPort << "\n";
}
