#ifndef DIAGNOSTICMESSAGEHANDLER_H
#define DIAGNOSTICMESSAGEHANDLER_H

#include "DoIPGenericHeaderHandler.h"

const int _DiagnosticPositiveACKLength = 5;
const int _DiagnosticMessageMinimumLength = 4;

unsigned char parseDiagnosticMessage(unsigned char sourceAddress [2], unsigned char data[64]);
unsigned char* createDiagnosticACK(PayloadType type, unsigned char sourceAddress [2], unsigned char targetAddress [2], unsigned char responseCode);
unsigned char* createDiagnosticMessage(unsigned char sourceAddress [2], unsigned char targetAddress [2], unsigned char* userData, int userDataLength);


#endif /* DIAGNOSTICMESSAGEHANDLER_H */