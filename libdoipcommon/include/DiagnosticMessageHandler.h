#ifndef DIAGNOSTICMESSAGEHANDLER_H
#define DIAGNOSTICMESSAGEHANDLER_H

#include "DoIPGenericHeaderHandler.h"
#include <functional>

using DiagnosticCallback = std::function<void(unsigned char*, unsigned char*, int)>;
using DiagnosticMessageNotification = std::function<bool(unsigned char*)>;

const int _DiagnosticPositiveACKLength = 5;
const int _DiagnosticMessageMinimumLength = 4;

unsigned char parseDiagnosticMessage(DiagnosticCallback cb, unsigned char sourceAddress [2], unsigned char data[64], int diagMessageLength);
unsigned char* createDiagnosticACK(bool ackType, unsigned char sourceAddress [2], unsigned char targetAddress [2], unsigned char responseCode);
unsigned char* createDiagnosticMessage(unsigned char sourceAddress [2], unsigned char targetAddress [2], unsigned char* userData, int userDataLength);


#endif /* DIAGNOSTICMESSAGEHANDLER_H */
