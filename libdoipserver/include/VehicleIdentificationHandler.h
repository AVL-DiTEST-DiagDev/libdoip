#ifndef VEHICLEIDENTIFICATIONHANDLER_H
#define VEHICLEIDENTIFICATIONHANDLER_H

#include "DoIPGenericHeaderHandler.h"


unsigned char* createVehicleIdentificationResponse(char* VIN,unsigned char* LogicalAdress,unsigned char* EID,unsigned char* GID,unsigned char FurtherActionReq);

const int _VIResponseLength = 32;

#endif /* VEHICLEIDENTIFICATIONHANDLER_H */

