#include "DoIPConnection.h"

#include <iostream>
#include <iomanip>

/**
 * Closes the connection by closing the sockets
 */
void DoIPConnection::aliveCheckTimeout() {
    std::cout << "Alive Check Timeout. Close Connection" << std::endl;
    closeSocket();
    close_connection();
}

/*
 * Closes the socket for this server
 */
void DoIPConnection::closeSocket() {
    close(tcpSocket);
    tcpSocket = 0;
}

/*
 * Receives a message from the client and calls reactToReceivedTcpMessage method
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPConnection::receiveTcpMessage() {
    int readedBytes = recv(tcpSocket, data, _MaxDataSize, 0);
    if(readedBytes > 0 && !aliveCheckTimer.timeout) {        
        //if alive check timouts should be possible, reset timer when message received
        if(aliveCheckTimer.active) {
            aliveCheckTimer.resetTimer();
        }
    
        int sendedBytes = reactToReceivedTcpMessage(readedBytes);
        
        return sendedBytes;
    } else {
        closeSocket();
        return 0;
    }
    return -1;
      
}

/*
 * Receives a message from the client and determine how to process the message
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPConnection::reactToReceivedTcpMessage(int readedBytes){

    dataLength = readedBytes;
    GenericHeaderAction action = parseGenericHeader(data, readedBytes);

    int sendedBytes;
    switch(action.type) {
        case PayloadType::NEGATIVEACK: {
            //send NACK
            sendedBytes = sendNegativeAck(action.value);

            if(action.value == _IncorrectPatternFormatCode || 
                    action.value == _InvalidPayloadLengthCode) {
                closeSocket();
                return -1;
            }

            return sendedBytes;
        }

        case PayloadType::ROUTINGACTIVATIONREQUEST: {
            //start routing activation handler with the received message
            unsigned char result = parseRoutingActivation(data);
            unsigned char clientAddress [2] = {data[8], data[9]};

            unsigned char* message = createRoutingActivationResponse(logicalGatewayAddress, clientAddress, result);
            sendedBytes = sendMessage(message, _GenericHeaderLength + _ActivationResponseLength);

            if(result == _UnknownSourceAddressCode || 
                    result == _UnsupportedRoutingTypeCode) {
                closeSocket();
                return -1;
            } else {
                //Routing Activation Request was successfull, save address of the client
                routedClientAddress = new unsigned char[2];
                routedClientAddress[0] = data[8];
                routedClientAddress[1] = data[9];

                //start alive check timer
                if(!aliveCheckTimer.active) {
                    aliveCheckTimer.cb = std::bind(&DoIPConnection::aliveCheckTimeout,this);
                    aliveCheckTimer.startTimer();
                }
            }

            return sendedBytes;
        }

        case PayloadType::ALIVECHECKRESPONSE: {
            return 0;
        }

        case PayloadType::DIAGNOSTICMESSAGE: {

            unsigned short target_address = 0;
            target_address |= ((unsigned short)data[10]) << 8U;
            target_address |= (unsigned short)data[11];
            bool ack = notify_application(target_address);

            if(ack)
                parseDiagnosticMessage(diag_callback, routedClientAddress, data, readedBytes - _GenericHeaderLength);

            break;
        }

        default: {
            std::cerr << "not handled payload type occured in receiveMessage()" << std::endl;
            return -1;
        }
    }  
    return -1;
}

void DoIPConnection::triggerDisconnection() {
    std::cout << "Application request to disconnect Client from Server" << std::endl;
    closeSocket();
}

/**
 * Sends a message back to the connected client
 * @param message           contains generic header and payload specific content
 * @param messageLength     length of the complete message
 * @return                  number of bytes written is returned,
 *                          or -1 if error occurred
 */
int DoIPConnection::sendMessage(unsigned char* message, int messageLength) {
    int result = write(tcpSocket, message, messageLength);
    return result;
}

/**
 * Sets the time in seconds after which a alive check timeout occurs.
 * Alive check timeouts can be deactivated when setting the seconds to 0
 * @param seconds   time after which alive check timeout occurs
 */
void DoIPConnection::setGeneralInactivityTime(uint16_t seconds) {
    if(seconds > 0) {
        aliveCheckTimer.setTimer(seconds);
    } else {
        aliveCheckTimer.disabled = true;
    }
}

/*
 * Send diagnostic message payload to the client
 * @param sourceAddress   logical source address (i.e. address of this server)
 * @param value     received payload
 * @param length    length of received payload
 */
void DoIPConnection::sendDiagnosticPayload(unsigned short sourceAddress, unsigned char* data, int length) {

    std::cout << "Sending diagnostic data: ";
    for(int i = 0; i < length; i++) {
        std::cout << std::hex << std::setw(2) << (unsigned int)data[i] << " ";
    }
    std::cout << std::endl;
    
    unsigned char* message = createDiagnosticMessage(sourceAddress, routedClientAddress, data, length);  
    sendMessage(message, _GenericHeaderLength + _DiagnosticMessageMinimumLength + length);
}


/*
 * Getter to the last received data
 * @return  pointer to the received data array
 */
const unsigned char* DoIPConnection::getData() {
    return data;
}

/*
 * Getter to the length of the last received data
 * @return  length of received data
 */
int DoIPConnection::getDataLength() const {
    return dataLength;
}

/*
 * Set the callback function for this doip server instance
 * @dc      Callback which sends the data of a diagnostic message to the application
 * @dmn     Callback which notifies the application of receiving a diagnostic message
 * @ccb     Callback for application function when the library closes the connection
 */
void DoIPConnection::setCallback(DiagnosticCallback dc, DiagnosticMessageNotification dmn, CloseConnectionCallback ccb) {
    diag_callback = dc;
    notify_application = dmn;
    close_connection = ccb;
}

void DoIPConnection::sendDiagnosticAck(unsigned short sourceAddress, bool ackType, unsigned char ackCode) {
    unsigned char data_TA [2] = { routedClientAddress[0], routedClientAddress[1] };
    
    unsigned char* message = createDiagnosticACK(ackType, sourceAddress, data_TA, ackCode);
    sendMessage(message, _GenericHeaderLength + _DiagnosticPositiveACKLength);
}

/**
 * Prepares a generic header nack and sends it to the client
 * @param ackCode       NACK-Code which will be included in the message
 * @return              amount of bytes sended to the client
 */
int DoIPConnection::sendNegativeAck(unsigned char ackCode) {
    unsigned char* message = createGenericHeader(PayloadType::NEGATIVEACK, _NACKLength);
    message[8] = ackCode;
    int sendedBytes = sendMessage(message, _GenericHeaderLength + _NACKLength);
    return sendedBytes;
}
