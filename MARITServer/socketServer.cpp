#include "stdafx.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#include "MARITEpt.h"
#include "VrtSDK10ex.h"

using namespace std;

#define WIN32_MEAN_AND_LEAN
const int REQ_WINSOCK_VER	= 2;

CRITICAL_SECTION Section;

bool param			= false;
int	 bodyCount		= 0;
int* nBodyCount		= &bodyCount;
char chName0[20]	= "";
char chName1[20]	= "";

// data containers for body#1
float rX			= 0;
float rY			= 0;
float rZ			= 0;
float phi			= 0;
float theta			= 0;
float psi			= 0;
float* a_rX			= &rX;
float* a_rY			= &rY;
float* a_rZ			= &rZ;
float* a_phi		= &phi;
float* a_theta		= &theta;
float* a_psi		= &psi;

char	bufroll[12];	
char	bufpitch[12];
char	bufyaw[12];

char	bufx[9];
char	bufy[9];
char	bufz[9];
char	space[2] = "_";
char	data[74]; 
//char	data[100];

// data containers for body#2
float rX2			= 0;
float rY2			= 0;
float rZ2			= 0;
float phi2			= 0;
float theta2		= 0;
float psi2			= 0;
float* a_rX2		= &rX2;
float* a_rY2		= &rY2;
float* a_rZ2		= &rZ2;
float* a_phi2		= &phi2;
float* a_theta2		= &theta2;
float* a_psi2		= &psi2;

char	bufroll2[12];	
char	bufpitch2[12];
char	bufyaw2[12];

char	bufx2[9];
char	bufy2[9];
char	bufz2[9];
char	space2[2] = "_";
char	data2[74];
//char	data2[100];

struct HandleConnection_parms {
	int port_number;
	SOCKET hsocket_id;
	char* dfName;
};

// socket connection handling threads
void HandleConnection(void*);
void HandleConnection2(void*);


void SetServerSockAddr(sockaddr_in* pSockAddr, int portNumber) {
	pSockAddr->sin_family = AF_INET;
	pSockAddr->sin_port = htons(portNumber);
	pSockAddr->sin_addr.S_un.S_addr = INADDR_ANY;
}


string GetHostDescription(const sockaddr_in &sockAddr) {
	ostringstream stream;
	stream << inet_ntoa(sockAddr.sin_addr) << ":" << ntohs(sockAddr.sin_port);
	return stream.str();
}


int StartSocketServer(int portNumber, int portNumber2) {	
	int iRet = 1;
	int i = 0;
	WSADATA wsaData;

	cout << "Initializing winsock... ";

	if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER, 0), &wsaData) == 0) {
		if (LOBYTE(wsaData.wVersion) >= REQ_WINSOCK_VER) {
			cout << "initialized.\n";

			SOCKET		hSocket	= INVALID_SOCKET,
						hSocket2 = INVALID_SOCKET;
			sockaddr_in sockAddr = {0};
			sockaddr_in sockAddr2 = {0};

			try {
				// create socket
				cout << "Creating sockets on server... ";
				if ((hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
					throw MARITEpt("could not create socket1.");

				if ((hSocket2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
					throw MARITEpt("could not create socket2.");

				cout << "sockets created.\n";

				// fill in server address
				SetServerSockAddr(&sockAddr, portNumber);
				SetServerSockAddr(&sockAddr2, portNumber2);
				
				//bind socket, socket is bound to the port number, not the IP of localhost
				cout << "Binding socket on port " << portNumber << "...";
				if (bind(hSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0)
					throw MARITEpt("could not bind socket1.");
				cout << "bound.\n";
				cout << "Binding socket on port " << portNumber2 << "...";
				if (bind(hSocket2, reinterpret_cast<sockaddr*>(&sockAddr2), sizeof(sockAddr2)) != 0)
					throw MARITEpt("could not bind socket2.");
				cout << "bound.\n";

				// listen to server socket
				cout << "Putting socket1 in listening mode... ";
				if (listen(hSocket, SOMAXCONN) != 0)
					throw MARITEpt("could not listen to server socket1.");
				cout << "done.\n";
				cout << "Putting socket2 in listening mode... ";
				if (listen(hSocket2, SOMAXCONN) != 0)
					throw MARITEpt("could not listen to server socket2.");
				cout << "done.\n";

				if (!ViconGetFrame(param))
					throw MARITEpt("Cannot get frame data, please check cameras.\n");
				/*ViconGetNumBodies(nBodyCount);
				for (i = 0; i < bodyCount; i++) {				
					ViconGetBodyName(i, chName[i]);
					cout << "	body number: " << i + 1 << " Name: " << chName[i] << endl;
				}*/
				ViconGetBodyName(0, chName0);
				ViconGetBodyName(1, chName1);
				// handle incoming requests here
	
				//cout << bodyCount << "ssss " << chName[0] << " ssss " << chName[1] << endl;
				struct HandleConnection_parms thread1_args;
				thread1_args.hsocket_id	 = hSocket;
				thread1_args.port_number = portNumber;
				thread1_args.dfName = chName0;

				struct HandleConnection_parms thread2_args;								
				thread2_args.port_number = portNumber2;
				thread2_args.hsocket_id	 = hSocket2;
				thread2_args.dfName = chName1; 
				//cout << "sssss " << thread1_args.dfName << " sssss " << thread2_args.dfName << endl;
				
				HANDLE hThreads[2];
				InitializeCriticalSection(&Section);
				hThreads[0] = (HANDLE)_beginthread(HandleConnection, 0, &thread1_args);
				hThreads[1] = (HANDLE)_beginthread(HandleConnection2, 0, &thread2_args);
				WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
				DeleteCriticalSection(&Section);

			}
			catch(MARITEpt e) {
				cerr << "\nError: " << e.what() << endl;
				iRet = 1;
			}

			if (hSocket != INVALID_SOCKET)
				closesocket(hSocket);
		}
		else
			cerr << "required version not supported.\n";

		cout << "Cleaning up winsock... ";

		// clean up winsock
		if (WSACleanup() != 0) {
			cerr << "cleanup failed.\n";
			iRet = 1;
		}

		cout << "done.\n";
	}
	else
		cerr << "startup failed!\n";	

	puts("Press any key to quit.\n");
	getchar();

	return iRet;
}




///////////// threads



void HandleConnection(void* arg) {	
	struct HandleConnection_parms* p = (struct HandleConnection_parms*)arg;
	sockaddr_in clientSockAddr;
	int clientSockSize = sizeof(clientSockAddr);

	while (1) {
		try {
			SOCKET hClientSocket	= INVALID_SOCKET;			
			hClientSocket = accept(p->hsocket_id, reinterpret_cast<sockaddr*>(&clientSockAddr), &clientSockSize);
			if (hClientSocket == INVALID_SOCKET){
				cout << "acception on port " << p->port_number << " failed.\n";
				throw MARITEpt("acception failed.");
			}
			cout << "\nNow sending position of " << p->dfName << " to port " << p->port_number << endl;	

			for (;;) {					
				ViconGetFrame(false);
				if (!ViconGetBodyEulerAngles(p->dfName, a_rX, a_rY, a_rZ, a_phi, a_theta, a_psi))
					throw MARITEpt("Failed to get frame data.\n");
				//cout << p->dfName << ":" << p->port_number << endl;
				//cout << "-------------------- new loop begins---------------------\n";
				//cout << "name: " << p->dfName << endl;
				//cout << "	X value: " << rX << endl;
				//cout << "	Y value: " << rY << endl;
				//cout << "	Z value: " << rZ << endl;
				//cout << "	Roll: " << phi << endl;		//X axis rotation value(roll)
				//cout << "	Pitch: " << theta << endl;	//Y axis rotation value(pitch)
				//cout << "	Yaw: " << psi << endl;		//Z axis rotation value(yaw)
				
				/*sprintf_s(bufroll, sizeof(bufroll), "%f", phi);  
				sprintf_s(bufpitch, sizeof(bufpitch), "%f", theta);
				sprintf_s(bufyaw, sizeof(bufyaw), "%f", psi);
				sprintf_s(bufx, sizeof(bufx), "%f", rX);
				sprintf_s(bufy, sizeof(bufy), "%f", rY);
				sprintf_s(bufz, sizeof(bufz), "%f", rZ);*/

				sprintf(bufroll, "%f", phi);  
				sprintf(bufpitch, "%f", theta);
				sprintf(bufyaw, "%f", psi);
				sprintf(bufx, "%f", rX);
				sprintf(bufy, "%f", rY);
				sprintf(bufz, "%f", rZ);
				/*gcvt(phi, 5, bufroll);
				gcvt(theta, 5, bufpitch);
				gcvt(psi, 5, bufyaw);
				gcvt(rX, 6, bufx);
				gcvt(rY, 6, bufy);
				gcvt(rZ, 6, bufz);*/

				/*cout << "above data converted to char[]: " << endl
					<< bufroll << endl << bufpitch << endl
					<< bufyaw << endl << bufx << endl
					<< bufy << endl << bufz << endl;	*/
				
				strcpy(data, bufroll);
				strcat(data, space);
				strcat(data, bufpitch);
				strcat(data, space);
				strcat(data, bufyaw);
				strcat(data, space);	
				strcat(data, bufx);
				strcat(data, space);
				strcat(data, bufy);
				strcat(data, space);
				strcat(data, bufz);
				
				cout << endl << "concatenated: " << endl << data << endl;
				if (send(hClientSocket, data, sizeof(data), 0) == SOCKET_ERROR)
					throw MARITEpt("Failed to send data, client may be closed.");
				Sleep(18);
				
			}					
	
		}
		catch(MARITEpt e) {
			cerr << "\nError: " << e.what() << endl;
			continue;	
		}
	}
}


/////// 2nd thread, do the same things as thread1, but saving to different variables

void HandleConnection2(void* arg) {
	
	struct HandleConnection_parms* q = (struct HandleConnection_parms*)arg;
	sockaddr_in clientSockAddr;
	int clientSockSize = sizeof(clientSockAddr);

	while (1) {
		try {
			SOCKET hClientSocket	= INVALID_SOCKET;			
			hClientSocket = accept(q->hsocket_id, reinterpret_cast<sockaddr*>(&clientSockAddr), &clientSockSize);
			if (hClientSocket == INVALID_SOCKET){
				cout << "acception on port " << q->port_number << " failed.\n";
				throw MARITEpt("acception failed.");
			}
			cout << "\nNow sending object " << q->dfName << "'s position to port " << q->port_number << endl;	

			for (;;) {					
				ViconGetFrame(false);
				if (!ViconGetBodyEulerAngles(q->dfName, a_rX2, a_rY2, a_rZ2, a_phi2, a_theta2, a_psi2))
					throw MARITEpt("Failed to get frame data.\n");
				//cout << q->dfName << ":" << q->port_number << endl;
				//cout << "-------------------- new loop begins---------------------\n";
				//cout << "name: " << q->dfName << endl;
				//cout << "	Roll: " << phi2 << endl;		//X axis rotation value(roll)
				//cout << "	Pitch: " << theta2 << endl;	//Y axis rotation value(pitch)
				//cout << "	Yaw: " << psi2 << endl;		//Z axis rotation value(yaw)
				//cout << "	X value: " << rX2 << endl;
				//cout << "	Y value: " << rY2 << endl;
				//cout << "	Z value: " << rZ2 << endl; 
			
				/*sprintf_s(bufroll2, sizeof(bufroll2), "%f", phi2);  
				sprintf_s(bufpitch2, sizeof(bufpitch2), "%f", theta2);
				sprintf_s(bufyaw2, sizeof(bufyaw2), "%f", psi2);
				sprintf_s(bufx2, sizeof(bufx2), "%f", rX2);
				sprintf_s(bufy2, sizeof(bufy2), "%f", rY2);
				sprintf_s(bufz2, sizeof(bufz2), "%f", rZ2);*/

				sprintf(bufroll2, "%f", phi2);  
				sprintf(bufpitch2, "%f", theta2);
				sprintf(bufyaw2, "%f", psi2);
				sprintf(bufx2, "%f", rX2);
				sprintf(bufy2, "%f", rY2);
				sprintf(bufz2, "%f", rZ2);
				/*
				gcvt(phi2, 5, bufroll2);
				gcvt(theta2, 5, bufpitch2);
				gcvt(psi2, 5, bufyaw2);
				gcvt(rX2, 6, bufx2);
				gcvt(rY2, 6, bufy2);
				gcvt(rZ2, 6, bufz2);	*/


		/*		cout << "above data converted to char[]: " << endl
					<< bufroll2 << endl << bufpitch2 << endl
					<< bufyaw2 << endl << bufx2 << endl
					<< bufy2 << endl << bufz2 << endl;	*/
				
				strcpy(data2, bufroll2);
				strcat(data2, space2);
				strcat(data2, bufpitch2);
				strcat(data2, space2);
				strcat(data2, bufyaw2);
				strcat(data2, space2);	
				strcat(data2, bufx2);
				strcat(data2, space2);
				strcat(data2, bufy2);
				strcat(data2, space2);
				strcat(data2, bufz2);
				
				//cout << endl << "concatenated: " << endl << data2 << endl;
				if (send(hClientSocket, data2, sizeof(data2), 0) == SOCKET_ERROR)
					throw MARITEpt("Failed to send data, client may be closed.");
				Sleep(1000);
			}			
	
		}
		catch(MARITEpt e) {
			cerr << "\nError: " << e.what() << endl;
			continue;	
		}
	}
}