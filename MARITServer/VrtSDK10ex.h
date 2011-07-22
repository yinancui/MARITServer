//-----------------------------------------------------------------------------
//	Vicon Real Time API.  Include file
//
//	Writen By:	Gus Lopez
//	Date:		3/14/06
//	
//	(c)2001 Vicon Motion Systems, Inc.
//
//	Description:  	
//-----------------------------------------------------------------------------

 bool ViconConnect(char *chIpAddr);
 bool ViconClose(void);
 bool ViconGetNumBodies( int *bodyNum );
 bool ViconGetNumMarkers( int *markerNum );
 bool ViconGetBodyName( int bodyNum, char *name );
 bool ViconGetMarkerName( int markerNum, char *name );
 bool ViconGetMarker( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, long int *a_rV );

 bool ViconGetBodyAngleAxis( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, float *a_rEX, float *a_rEY, float *a_rEZ );
 bool ViconGetBodyEulerAngles( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, float *a_rEX, float *a_rEY, float *a_rEZ);
 bool ViconGetBodyQuaternion( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, float *a_rQX, float *a_rQY, float *a_rQZ, float *a_rQW);
 bool ViconGetBodyRotationMatrix( char * a_pName, float a_rGlobalRotationMatrix[3][3]);

 bool ViconGetFrame(bool getUnlabeled);
 bool ViconIsConnected(void);
 void ViconGetLastError(char *chError);
 bool ViconGetFrameTimeStamp(double *dTimeStamp);

 bool ViconGetNumLocalBodies( int *bodyNum );
 bool ViconGetLocalBodyAngleAxis( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, float *a_rEX, float *a_rEY, float *a_rEZ);
 bool ViconGetLocalBodyEulerAngles( char * a_pName, float *a_rX, float *a_rY, float *a_rZ, float *a_rEX, float *a_rEY, float *a_rEZ);

 void ViconSetYup();
