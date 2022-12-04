#ifndef DIAGNOSTICMESSAGEHANDLER_H
#define DIAGNOSTICMESSAGEHANDLER_H

#include "DoIPGenericHeaderHandler.h"
#include <functional>

using DiagnosticCallback = std::function<void(unsigned short, unsigned char*, int)>;
using DiagnosticMessageNotification = std::function<bool(unsigned short)>;

const int _DiagnosticPositiveACKLength = 5;
const int _DiagnosticMessageMinimumLength = 4;

const unsigned char _ValidDiagnosticMessageCode = 0x00;
const unsigned char _InvalidSourceAddressCode = 0x02;
const unsigned char _UnknownTargetAddressCode = 0x03;

unsigned char parseDiagnosticMessage(DiagnosticCallback callback, unsigned char sourceAddress [2], unsigned char* data, int diagMessageLength);
unsigned char* createDiagnosticACK(bool ackType, unsigned short sourceAddress, unsigned char targetAddress [2], unsigned char responseCode);
unsigned char* createDiagnosticMessage(unsigned short sourceAddress, unsigned char targetAddress [2], unsigned char* userData, int userDataLength);


#endif /* DIAGNOSTICMESSAGEHANDLER_H */
