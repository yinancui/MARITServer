#ifndef PC2RC_H_
#define PC2RC_H_

enum serial_parity  { spNONE,    spODD, spEVEN };

void sendArray(char *buffer, int len);
int initializeSerialPort(char *port_arg, int rate_arg, serial_parity parity_arg);
void closeSerialPort();
int normalized_to_pc2rc2(double cntlcmd, int troff, double slp);

#endif