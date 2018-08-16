/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VehicleIdentificationHandler.h
 * Author: linuxvm
 *
 * Created on 13. August 2018, 16:51
 */

#ifndef VEHICLEIDENTIFICATIONHANDLER_H
#define VEHICLEIDENTIFICATIONHANDLER_H


#include "DoIPGenericHeaderHandler.h"


unsigned char* createVehicleIdentificationResponse(char* VIN,unsigned char* LogicalAdress,unsigned char* EID,unsigned char* GID,unsigned char FurtherActionReq);

const int _VIResponseLength = 32;

#endif /* VEHICLEIDENTIFICATIONHANDLER_H */

