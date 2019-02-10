#ifndef ROUTINGACTIVATIONHANDLER_H
#define ROUTINGACTIVATIONHANDLER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include "DoIPGenericHeaderHandler.h"

const int _ActivationResponseLength = 9;

const unsigned char _UnknownSourceAddressCode = 0x00;
const unsigned char _UnsupportedRoutingTypeCode = 0x06;
const unsigned char _SuccessfullyRoutedCode = 0x10;

unsigned char parseRoutingActivation(unsigned char* data);
unsigned char* createRoutingActivationResponse(unsigned char clientAddress[2],
                                                unsigned char responseCode);
bool checkSourceAddress(uint32_t address);


#endif /* ROUTINGACTIVATIONHANDLER_H */

