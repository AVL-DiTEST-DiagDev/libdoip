#ifndef DOIPGENERICHEADERHANDLER_H
#define DOIPGENERICHEADERHANDLER_H

#include <stdint.h>

const int _GenericHeaderLength = 8;
const int _NACKLength = 1;

const unsigned char _IncorrectPatternFormatCode = 0x00;
const unsigned char _UnknownPayloadTypeCode = 0x01;
const unsigned char _InvalidPayloadLengthCode = 0x04;

enum PayloadType {
    NEGATIVEACK,
    ROUTINGACTIVATIONREQUEST,
    ROUTINGACTIVATIONRESPONSE,
    VEHICLEIDENTREQUEST,
    VEHICLEIDENTRESPONSE,
    DIAGNOSTICMESSAGE,
    DIAGNOSTICPOSITIVEACK,
    DIAGNOSTICNEGATIVEACK,
    ALIVECHECKRESPONSE,
};

struct GenericHeaderAction {
    PayloadType type;
    unsigned char value;
};

GenericHeaderAction parseGenericHeader(unsigned char* data, int dataLenght);
unsigned char* createGenericHeader(PayloadType type, uint32_t length);


#endif /* DOIPGENERICHEADERHANDLER_H */

