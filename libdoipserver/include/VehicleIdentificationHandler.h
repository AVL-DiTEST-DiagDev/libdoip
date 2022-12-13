#ifndef VEHICLEIDENTIFICATIONHANDLER_H
#define VEHICLEIDENTIFICATIONHANDLER_H

#include "DoIPGenericHeaderHandler.h"
#include <string>


unsigned char* createVehicleIdentificationResponse(std::string VIN,unsigned short LogicalAdress,unsigned char* EID,unsigned char* GID,unsigned char FurtherActionReq);

const int _VIResponseLength = 32;

#endif /* VEHICLEIDENTIFICATIONHANDLER_H */

