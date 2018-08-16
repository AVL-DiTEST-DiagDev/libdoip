#include "DoIPGenericHeaderHandler.h"
#include <iostream>
using namespace std;

/**
 * Checks if the received Generic Header is valid
 * @param data          message which was received
 * @param dataLenght    length of the message
 * @return              Returns a GenericHeaderAction struct, which stores the
 *                      payload type and a byte for further message processing
 */
GenericHeaderAction parseGenericHeader(unsigned char data[64], int dataLenght) {
    
    
    
    GenericHeaderAction action;
    
    //Check Generic DoIP synchronization pattern
    if((int)(data[1] ^ (0xFF)) != (int)data[0]) {
        //Return Error, Protocol Version not correct
        action.type = PayloadType::NEGATIVEACK;
        action.value = 0x00;
        return action;
    }
    
    //Check Payload Type
    //Value of RoutingActivationRequest = 0x0005
    if(data[2] == 0x00 && data[3] == 0x05) {
        action.type = PayloadType::ROUTINGACTIVATIONREQUEST;
    } 
     //Value of Vehicle Identification Request = 0x0001
    else if(data[2] == 0x00 && data[3] == 0x01) {
        action.type = PayloadType::VEHICLEIDENTREQUEST;
    }
    else {
        //Unknown Payload Type --> Send Generic DoIP Header NACK
        action.type = PayloadType::NEGATIVEACK;
        action.value = 0x01;
        return action;
    }
    
   
    
    
    
    //Check if the message length exceeds the maximum processable length
    if(dataLenght > 64) {
        action.type = PayloadType::NEGATIVEACK;
        action.value = 0x02;
        return action;
    }
    
    //Check current DoIP protocol handler memory
    //DoIP-044 - Figure 7
    
    //Check Payload Type specific length
    switch(action.type) {
        case PayloadType::ROUTINGACTIVATIONREQUEST: {
            if(dataLenght - _GenericHeaderLength != 7) {
                action.type = PayloadType::NEGATIVEACK;
                action.value = 0x04;
                return action;
            }
            break;
        }
        case PayloadType::VEHICLEIDENTREQUEST: { //PayloadTypeLength = 0
            if(dataLenght - _GenericHeaderLength != 0) {
                action.type = PayloadType::NEGATIVEACK;
                action.value = 0x04;
                return action;
            }
        }
    }
    
    return action;
}

/**
 * Creates a generic header
 * @param type      payload type which will be filled in the header
 * @param length    length of the payload type specific message
 * @return          header array
 */
unsigned char* createGenericHeader(PayloadType type, uint32_t length) {
    unsigned char *header = new unsigned char[8 + length];
    header[0] = 0x02;
    header[1] = 0xFD;
    switch(type) {
        case PayloadType::ROUTINGACTIVATIONRESPONSE: {
            header[2] = 0x00;
            header[3] = 0x06;
            break;
        }
        case PayloadType::NEGATIVEACK: {
            header[2] = 0x00;
            header[3] = 0x00;
            break;
        }
        case PayloadType::VEHICLEIDENTRESPONSE:{
            header[2] = 0x00;
            header[3] = 0x04;
        }
    }
    
    header[4] = (length >> 24) & 0xFF;
    header[5] = (length >> 16) & 0xFF;
    header[6] = (length >> 8) & 0xFF;
    header[7] = length & 0xFF;
    
    return header;
}
