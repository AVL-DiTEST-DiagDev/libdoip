#ifndef ROUTINGACTIVATIONHANDLER_H
#define ROUTINGACTIVATIONHANDLER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include "DoIPGenericHeaderHandler.h"

const int _ActivationResponseLength = 9;

unsigned char parseRoutingActivation(unsigned char data[64]);
unsigned char* createRoutingActivationResponse(unsigned char clientAddress[2],
                                                unsigned char responseCode);
bool checkSourceAddress(uint32_t address);


#endif /* ROUTINGACTIVATIONHANDLER_H */

