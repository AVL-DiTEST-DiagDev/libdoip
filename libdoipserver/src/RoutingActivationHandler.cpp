#include "RoutingActivationHandler.h"
#include <iostream>

/**
 * Checks if the Routing Activation Request is valid
 * @param data  contains the request
 * @return      routing activation response code 
 */
unsigned char parseRoutingActivation(unsigned char data[64]) {
    
    //Check if source address is known
    uint32_t address = 0;
    address |= (uint32_t)data[8] << 8;
    address |= (uint32_t)data[9];
    if(!checkSourceAddress(address)) {
        //send routing activation negative response code --> close socket
        return 0x00;
    }
    
    //Check if routing activation type is supported
    switch(data[10]) {
        case 0x00: {
            //Activation Type default
            break;
        }
        case 0x01: {
            //Activation Type WWH-OBD
            break;
        }
        default: {
            //unknown activation type
            //send routing activation negative response code --> close socket
            return 0x06;
        }
    }
    
    //Call Socket handler    
    //Check if authentication is required   
    //Check if confirmation is required
    
    //if not exited before, send routing activation positive response
    return 0x10;
}

/**
 * Create the complete routing activation response, which also contains the
 * generic header
 * @param clientAddress     address of the test equipment
 * @param responseCode      routing activation response code
 * @return                  complete routing activation response
 */
unsigned char* createRoutingActivationResponse(unsigned char clientAddress[2],
                                                unsigned char responseCode) {
    unsigned char* message = createGenericHeader(PayloadType::ROUTINGACTIVATIONRESPONSE,
                                                    _ActivationResponseLength);
    
    //Logical address of external test equipment
    message[8] = clientAddress[0];
    message[9] = clientAddress[1];
    
    //logical address of DoIP entity
    message[10] = 0x01;
    message[11] = 0x00;
    
    //routing activation response code
    message[12] = responseCode;
    
    //reserved for future standardization use
    message[13] = 0x00;
    message[14] = 0x00;
    message[15] = 0x00;
    message[16] = 0x00;           
    
    return message;
}

/**
 * Checks if the submitted address is valid
 * @param address	the address which will be checked
 * @return			true if address is valid
 */
bool checkSourceAddress(uint32_t address) {
    uint32_t minAddress = 3584;  // 0x0E00
    uint32_t maxAddress = 4095;     // 0x0FFF
    
    for(uint32_t i = minAddress; i <= maxAddress; i++) {
        if(address == i) {
            return true;
        }
    }
           
    return false;
}