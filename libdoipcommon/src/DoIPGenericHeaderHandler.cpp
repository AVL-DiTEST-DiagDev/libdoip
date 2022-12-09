#include "DoIPGenericHeaderHandler.h"
#include <iostream>
#include <iomanip>

using namespace std;

/**
 * Checks if the received Generic Header is valid
 * @param data          message which was received
 * @param dataLenght    length of the message
 * @return              Returns a GenericHeaderAction struct, which stores the
 *                      payload type and a byte for further message processing
 */
GenericHeaderAction parseGenericHeader(unsigned char* data, int dataLenght) {
    
    GenericHeaderAction action;
    
    //Only check header if received data is greater or equals the set header length
    if(dataLenght >= _GenericHeaderLength) {
        
        //Check Generic DoIP synchronization pattern
        if((int)(data[1] ^ (0xFF)) != (int)data[0]) {
            //Return Error, Protocol Version not correct
            action.type = PayloadType::NEGATIVEACK;
            action.value = _IncorrectPatternFormatCode;
            action.payloadLength = 0;
            return action;
        }

        unsigned int payloadLength = 0;
        payloadLength |= (unsigned int)(data[4] << 24);
        payloadLength |= (unsigned int)(data[5] << 16);
        payloadLength |= (unsigned int)(data[6] <<  8);
        payloadLength |= (unsigned int)(data[7] <<  0);

        action.payloadLength = payloadLength;

        //Check Payload Type
        PayloadType messagePayloadType;
        if(data[2] == 0x00 && data[3] == 0x05) {	//Value of RoutingActivationRequest = 0x0005
            messagePayloadType = PayloadType::ROUTINGACTIVATIONREQUEST;
        }
        else if(data[2] == 0x00 && data[3] == 0x04){
            messagePayloadType = PayloadType::VEHICLEIDENTRESPONSE;
        }
        else if(data[2] == 0x00 && data[3] == 0x01) {   //Value of Vehicle Identification Request = 0x0001
            messagePayloadType = PayloadType::VEHICLEIDENTREQUEST;
        }
        else if(data[2] == 0x80 && data[3] == 0x01) {   //Value of Diagnose Message = 0x8001
            messagePayloadType = PayloadType::DIAGNOSTICMESSAGE;
        } 
        else if(data[2] == 0x80 && data[3] == 0x02) {   //Value of Diagnostic Message positive ack = 0x8002
            messagePayloadType = PayloadType::DIAGNOSTICPOSITIVEACK;
        } 
        else if(data[2] == 0x80 && data[3] == 0x03) {   //Value of Diagnostic Message negative ack = 0x8003
            messagePayloadType = PayloadType::DIAGNOSTICNEGATIVEACK;
        } else {
            //Unknown Payload Type --> Send Generic DoIP Header NACK
            action.type = PayloadType::NEGATIVEACK;
            action.value = _UnknownPayloadTypeCode;
            return action;
        }

        //Check Payload Type specific length
        switch(messagePayloadType) {
            case PayloadType::ROUTINGACTIVATIONREQUEST: {
                if(payloadLength != 7 && payloadLength != 11) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                    return action;
                }
                break;
            }

            case PayloadType::ALIVECHECKRESPONSE: {
                if(payloadLength != 2) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                    return action;
                }
                break;
            }
            
            case PayloadType::VEHICLEIDENTREQUEST: {
                if(payloadLength != 0) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                    return action;
                }
                break;
            }

            case PayloadType::VEHICLEIDENTRESPONSE:{
                if(payloadLength != 32 && payloadLength != 33) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                    return action;
                }
                break;
            }

            case PayloadType::DIAGNOSTICMESSAGE: {
                if(payloadLength <= 4) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                    return action;
                }
                break;	
            }

            case PayloadType::DIAGNOSTICPOSITIVEACK: {
                if(payloadLength < 5) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                }
                break;
            }

            case PayloadType::DIAGNOSTICNEGATIVEACK: {
                if(payloadLength < 5) {
                    action.type = PayloadType::NEGATIVEACK;
                    action.value = _InvalidPayloadLengthCode;
                }
                break;
            }

            default: {
                std::cerr << "not handled payload type occured in parseGenericHeader()" << std::endl;
                break;	
            }
        }
        action.type = messagePayloadType;
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
            break;
        }

        case PayloadType::DIAGNOSTICMESSAGE: {
            header[2] = 0x80;
            header[3] = 0x01;
            break;
        }

        case PayloadType::DIAGNOSTICPOSITIVEACK: {
            header[2] = 0x80;
            header[3] = 0x02;
            break;
        }

        case PayloadType::DIAGNOSTICNEGATIVEACK: {
            header[2] = 0x80;
            header[3] = 0x03;
            break;
        }
        
        case PayloadType::ALIVECHECKRESPONSE: {
            header[2] = 0x00;
            header[3] = 0x08;
            break;
        }

        default: {
            std::cerr << "not handled payload type occured in createGenericHeader()" << std::endl;
            break;
        }
    }
    
    header[4] = (length >> 24) & 0xFF;
    header[5] = (length >> 16) & 0xFF;
    header[6] = (length >> 8) & 0xFF;
    header[7] = length & 0xFF;
    
    return header;
}
