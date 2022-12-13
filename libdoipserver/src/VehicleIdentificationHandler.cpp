#include "VehicleIdentificationHandler.h"
#include <iostream>



unsigned char* createVehicleIdentificationResponse(std::string VIN,unsigned short LogicalAddress, 
                                                    unsigned char* EID, unsigned char* GID,
                                                    unsigned char FurtherActionReq) //also used für the Vehicle Announcement
{
    unsigned char* message = createGenericHeader(PayloadType::VEHICLEIDENTRESPONSE, _VIResponseLength);
    
    //VIN Number 
    int j = 0;
    for(int i = 8; i <= 24; i++)
    {      
        message[i] = (unsigned char)VIN[j];
        j++;
    }
    
    //Logical Adress
    message[25] = (LogicalAddress >> 8) & 0xFF;
    message[26] = LogicalAddress & 0xFF;    
    
    //EID
    j = 0;
    for(int i = 27; i <= 32; i++)
    {
        message[i] = EID[j];
        j++;
    }
    
    //GID
    j = 0;
    for(int i = 33; i <= 38; i++)
    {
        message[i] = GID[j];
        j++;
    }
    
    message[39] = FurtherActionReq;
    
    return message;
}

