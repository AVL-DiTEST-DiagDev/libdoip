#ifndef DOIPGENERICHEADERHANDLER_H
#define DOIPGENERICHEADERHANDLER_H

#include <stdint.h>

const int _GenericHeaderLength = 8;
const int _NACKLength = 1;


enum PayloadType { 
    NEGATIVEACK,
    ROUTINGACTIVATIONREQUEST,
    ROUTINGACTIVATIONRESPONSE,
    VEHICLEIDENTREQUEST,
    VEHICLEIDENTRESPONSE
};

struct GenericHeaderAction {
    PayloadType type;
    unsigned char value;
};

GenericHeaderAction parseGenericHeader(unsigned char data[64], int dataLenght);
unsigned char* createGenericHeader(PayloadType type, uint32_t length);


#endif /* DOIPGENERICHEADERHANDLER_H */

