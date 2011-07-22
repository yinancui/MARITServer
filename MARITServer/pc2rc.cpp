#include "stdafx.h"
#include "pc2rc.h"
#include <windows.h>
#include <iostream>

char              port[10];                      // port name "com1",...
int               rate;                          // baudrate
serial_parity     parityMode;
HANDLE            serial_handle;
//HANDLE            sThread;

//control c;
int normalized_to_pc2rc2(double cntlcmd, int troff, double slp)
{
  int y = 15000 + troff + ((int) (cntlcmd * slp));
  if ((y % 2 == 0) && (y != 15000))
    y += 1;

  return y;
}

void sendArray(char *buffer, int len)
{
    unsigned long result;

	if (serial_handle!=INVALID_HANDLE_VALUE) {
		//WriteFile(serial_handle, buffer, len, &result, NULL);
		
        if (!WriteFile(serial_handle, buffer, len, &result, NULL)) // WriteFile returns nonzero when succeeds
			std::cout << "Failed to write to serial port.\n";
		else {
			std::cout << "Writing to serial port was successful.\n";
			//std::cout << buffer << std::endl;
		}
	}
}



int initializeSerialPort(char *port_arg, int rate_arg, serial_parity parity_arg)
{
    int error;
	DCB  dcb = {0};   //clear all its fields
    COMMTIMEOUTS cto = { 0, 0, 0, 1, 1 };

    /* --------------------------------------------- */
    if (serial_handle!=INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;

    error = 0;

    if (port_arg!=0)
    {
        strncpy_s(port, port_arg, 10); // pass port_arg to port, and from port to serial_handle later
        rate      = rate_arg;
        parityMode= parity_arg;
        memset(&dcb,0,sizeof(dcb));

        /* -------------------------------------------------------------------- */
        // set DCB to configure the serial port
        dcb.DCBlength       = sizeof(dcb);                   
        
        /* ---------- Serial Port Config ------- */
        dcb.BaudRate        = rate;

        switch(parityMode)
        {
            case spNONE:
                            dcb.Parity      = NOPARITY;
                            dcb.fParity     = 0;
                            break;
            case spEVEN:
                            dcb.Parity      = EVENPARITY;
                            dcb.fParity     = 1;
                            break;
            case spODD:
                            dcb.Parity      = ODDPARITY;
                            dcb.fParity     = 1;
                            break;
        }


        dcb.StopBits        = ONESTOPBIT;
        dcb.ByteSize        = 8;
        
        dcb.fOutxCtsFlow    = 0;
        dcb.fOutxDsrFlow    = 0;
        dcb.fDtrControl     = DTR_CONTROL_DISABLE;
        dcb.fDsrSensitivity = 0;
        dcb.fRtsControl     = RTS_CONTROL_DISABLE;
        dcb.fOutX           = 0;
        dcb.fInX            = 0;
        
        /* ----------------- misc parameters ----- */
        dcb.fErrorChar      = 0;
        dcb.fBinary         = 1;
        dcb.fNull           = 0;
        dcb.fAbortOnError   = 0;
        dcb.wReserved       = 0;
        dcb.XonLim          = 2;
        dcb.XoffLim         = 4;
        dcb.XonChar         = 0x13;
        dcb.XoffChar        = 0x19;
        dcb.EvtChar         = 0;
        
        /* -------------------------------------------------------------------- */
        serial_handle    = CreateFileA(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,NULL,NULL); //changed from CreateFile()
                   // opening serial port


        if (serial_handle    != INVALID_HANDLE_VALUE)
        {
            if(!SetCommMask(serial_handle, 0))
                error = 1;

            // set timeouts
            if(!SetCommTimeouts(serial_handle,&cto))
                error = 2;

            // set DCB
            if(!SetCommState(serial_handle,&dcb))
                error = 4;
        }
        else
            error = 8;
    }
    else
        error = 16;


    /* --------------------------------------------- */
    if (error!=0)
    {
        CloseHandle(serial_handle);
        serial_handle = INVALID_HANDLE_VALUE;		
    }
	/*else
    {
        //All went well, start the command sender thread
        unsigned dwThreadId = 0;
		sThread = (HANDLE) _beginthreadex(NULL, 0,&cmdSendThread,NULL, 0,&dwThreadId);  //by weizhong
    }
	*/
    
    
    return(error);
}



void closeSerialPort()
{
    //TerminateThread(sThread,0);
    if (serial_handle!=INVALID_HANDLE_VALUE)
            CloseHandle(serial_handle);
        serial_handle = INVALID_HANDLE_VALUE;
}

