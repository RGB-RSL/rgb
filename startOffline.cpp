#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
using namespace std;
const string FIRE_BRIGADE = "FireBrigade", FIRE_STATION = "FireStation", AMBULANCE_TEAM = "AmbulanceTeam", AMBULANCE_CENTER = "AmbulanceCenter", POLICE_FORCE = "PoliceForce", POLICE_OFFICE = "PoliceOffice";
const string BINARY_NAME = "EskilasAgent";

int main(int argc, char *argv[])
{
	//doOffline for {FB, FS, AT, AC, PF, PO}
	bool doOffline[6] = {true, true, true, true, true, true};
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '0')
			doOffline[i - 1] = false;
	}
	system("ls LogFiles/Offline/ > offlineList");
	ifstream fin("offlineList");
	while(!fin.eof())
	{
		string s;
		fin >> s;
		string type = s.substr(0, s.find_first_of('-'));
		if(type == FIRE_BRIGADE)
		{
			if(doOffline[0])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 1 offline " + id;
				system(command.c_str());
			}
		}
		if(type == FIRE_STATION)
		{
			if(doOffline[1])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 2 offline " + id;
				system(command.c_str());
			}
		}
		if(type == AMBULANCE_TEAM)
		{
			if(doOffline[2])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 3 offline " + id;
				system(command.c_str());
			}
		}
		if(type == AMBULANCE_CENTER)
		{
			if(doOffline[3])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 4 offline " + id;
				system(command.c_str());
			}
		}
		if(type == POLICE_FORCE)
		{
			if(doOffline[4])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 5 offline " + id;
				system(command.c_str());
			}
		}
		if(type == POLICE_OFFICE)
		{
			if(doOffline[5])
			{
				string id = s.substr(s.find_first_of('-') + 1, s.find_first_of('.') - s.find_first_of('-') - 1);
				string command = "./" + BINARY_NAME + " 6 offline " + id;
				system(command.c_str());
			}
		}
	}
	return 0;
}
