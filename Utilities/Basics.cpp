
#include "Basics.h"
#include <cmath>
#include <cstdarg>
#include <cassert>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstring>

using namespace std;

#define LOGLEVEL 0


// Config_PList::WarnLoading ======

void basics::WarnLoadingTrue(string str)
{
	//	int c = system(("echo -n ''" + str).c_str());
	//	for(unsigned i = 0; i < (100 - str.size()); i++)
	//		c = system("echo -n ' '");
	//	c = system("echo -n '['\\\\033[0\\;32m'   OK   '\\\\033[0\\;39m']\n'");
}

void basics::WarnLoadingFalse(string str)
{
	//	int c = system(("echo -n ''" + str).c_str());
	//	for(unsigned i = 0; i < (100 - str.size()); i++)
	//		c = system("echo -n ' '");
	//	c = system("echo -n '['\\\\033[0\\;31m' Failed '\\\\033[0\\;39m']\n'");
}
