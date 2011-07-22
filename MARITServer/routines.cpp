#include "stdafx.h"
#include "routines.h"
#include "VrtSDK10ex.h"
#include <iostream>
#include "MARITEpt.h"

using namespace std;

//bool param			= false;
//int	 bodyCount		= 0;
//int* nBodyCount		= &bodyCount;
//char chName[2][20];

bool startVicon(char* serverIP) {
	bool retVal = true;	
	bool t = true;
	int i = 0;
	if (!ViconIsConnected()) {
		try {		
			cout << "Connecting to Vicon datastation... ";			
			if (!ViconConnect(serverIP)) {
				throw MARITEpt("cannot connect to Vicon datastation.\n");
			}
			cout << "connected.\n";
			//cout << "Collecting testbed information... \n";
			//if (!ViconGetFrame(param))
			//	throw MARITEpt("Cannot get frame data, please check cameras.\n");		
			//ViconGetNumBodies(nBodyCount);
			//cout << "Number of bodies in the testbed: " << bodyCount << endl;
			//// print body name table
			//for (i = 0; i < bodyCount; i++) {				
			//	ViconGetBodyName(i, chName[i]);
			//	cout << "	body number: " << i + 1 << " Name: " << chName[i] << endl;
			//}
			cout << "\n" << "************************************\n";
		}
		catch(MARITEpt e) {
			cerr << "\nError: " << e.what() << endl;
			retVal = false;
		}
	}

	return retVal;
}