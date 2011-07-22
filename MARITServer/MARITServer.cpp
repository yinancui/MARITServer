// MARITServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "socketServer.h"
#include "routines.h"
#include "MARITEpt.h"

using namespace std;

char* serverIP = "192.168.1.10";
const int DEFAULT_PORT		= 6666;
const int DEFAULT_PORT2		= 8888;

int _tmain(int argc, _TCHAR* argv[])
{
	int retVal = 0;	
	cout << "***************************************\n"
		 << "         Welcome to MARIT!\n"
		 << "***************************************\n\n";
	
	try {	
		if (!startVicon(serverIP))
			throw MARITEpt("Failed to start Vicon connection.\n");
		StartSocketServer(DEFAULT_PORT, DEFAULT_PORT2);
	}
	catch(MARITEpt e) {
			cerr << "\nError: " << e.what() << endl;			
			retVal = 1;
	}

	puts("Press any key to quit... \n");
	getchar();
	return retVal;
}

