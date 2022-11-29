#include "DiagnosticMessageHandler.h"
#include <iostream>
#include <cstring>

/**
 * Checks if a received Diagnostic Message is valid
 * @param cb                    callback which will be called with the user data
 * @param sourceAddress		currently registered source address on the socket
 * @param data			message which was received
 * @param diagMessageLength     length of the diagnostic message
 */
unsigned char parseDiagnosticMessage(DiagnosticCallback callback, unsigned char sourceAddress [2],
                                    unsigned char* data, int diagMessageLength) {
    
    if(diagMessageLength >= _DiagnosticMessageMinimumLength) {
        //Check if the received SA is registered on the socket
        if(data[8] != sourceAddress[0] || data[9] != sourceAddress[1]) {
            //SA of received message is not registered on this TCP_DATA socket
            return _InvalidSourceAddressCode;
        }

        //Pass the diagnostic message to the target network/transport layer
        unsigned char target_address [2] = {data[10], data[11]};

        int cb_message_length = diagMessageLength - _DiagnosticMessageMinimumLength;
        unsigned char* cb_message = new unsigned char[cb_message_length];

        for(int i = _DiagnosticMessageMinimumLength; i < diagMessageLength; i++) {
            cb_message[i - _DiagnosticMessageMinimumLength] = data[8+ i];
        }

        callback(target_address, cb_message, cb_message_length);

        //return positive ack code
        return _ValidDiagnosticMessageCode;
    }
    return _UnknownTargetAddressCode;
}

/**
 * Creates a diagnostic message positive/negative acknowledgment message
 * @param type                  defines positive/negative acknowledge type
 * @param sourceAddress		logical address of the receiver of the previous diagnostic message
 * @param targetAddress		logical address of the sender of the previous diagnostic message
 * @param responseCode		positive or negative acknowledge code
 * @return pointer to the created diagnostic message acknowledge
 */
unsigned char* createDiagnosticACK(bool ackType, unsigned short sourceAddress, 
                                    unsigned char targetAddress [2], unsigned char responseCode) {
    
    PayloadType type;
    if(ackType)
        type = PayloadType::DIAGNOSTICPOSITIVEACK;
    else
        type = PayloadType::DIAGNOSTICNEGATIVEACK;
    
    unsigned char* message = createGenericHeader(type, _DiagnosticPositiveACKLength);

    //add source address to the message
    message[8] = (unsigned char)((sourceAddress >> 8) & 0xFF);
    message[9] = (unsigned char)(sourceAddress & 0xFF);

    //add target address to the message
    message[10] = targetAddress[0];
    message[11] = targetAddress[1];

    //add positive or negative acknowledge code to the message
    message[12] = responseCode;

    return message;
}

/**
 * Creates a complete diagnostic message
 * @param sourceAddress		logical address of the sender of a diagnostic message
 * @param targetAddress		logical address of the receiver of a diagnostic message
 * @param userData		actual diagnostic data
 * @param userDataLength	length of diagnostic data
 */
unsigned char* createDiagnosticMessage(unsigned short sourceAddress, unsigned char targetAddress [2],
                                        unsigned char* userData, int userDataLength) {
    
    unsigned char* message = createGenericHeader(PayloadType::DIAGNOSTICMESSAGE, _DiagnosticMessageMinimumLength + userDataLength);

    //add source address to the message
    message[8] = (unsigned char)((sourceAddress >> 8) & 0xFF);
    message[9] = (unsigned char)(sourceAddress & 0xFF);

    //add target address to the message
    message[10] = targetAddress[0];
    message[11] = targetAddress[1];

    //add userdata to the message
    for(int i = 0; i < userDataLength; i++) {
        message[12 + i] = userData[i];	
    }

    return message;
}
