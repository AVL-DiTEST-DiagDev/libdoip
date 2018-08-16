#include "DoIPServer.h"

/*
 * Set up a socket till the point where the server waits till a client
 * approaches to make a connection
 */
void DoIPServer::setupSocket() {
    
    sockfd_receiver = socket(AF_INET, SOCK_STREAM, 0);
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAdress.sin_port = htons(_ServerPort);
    
    //binds the socket to the address and port number
    bind(sockfd_receiver, (struct sockaddr *)&serverAdress, sizeof(serverAdress));     
    //waits till client approach to make connection
    listen(sockfd_receiver, 5);                                                          
    
    sockfd_sender = accept(sockfd_receiver, (struct sockaddr*) NULL, NULL);
}

void DoIPServer::setupUdpSocket(){
    
    sockfd_receiver = socket(AF_INET, SOCK_DGRAM, 0);
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAdress.sin_port = htons(_ServerPort);
    
    //binds the socket to the address and port number
    bind(sockfd_receiver, (struct sockaddr *)&serverAdress, sizeof(serverAdress)); 
    
}

/*
 * Closes the socket for this server
 */
void DoIPServer::closeSocket() {
    close(sockfd_receiver);
    close(sockfd_sender);
}

/*
 * Receives a message from the client and determine how to process the message
 */
void DoIPServer::receiveMessage() {
    
    int readedBytes;
    while(true) {
        readedBytes = recv(sockfd_sender, data, _MaxDataSize, 0);

        if(readedBytes > 0) {

            GenericHeaderAction action = parseGenericHeader(data, readedBytes);
           
            switch(action.type) {
                case PayloadType::NEGATIVEACK: {
                    //send NACK
                    unsigned char* message = createGenericHeader(action.type, _NACKLength);
                    message[8] = action.value;
                    sendMessage(message, _GenericHeaderLength + _NACKLength);
                    
                    if(action.value == 0x00 || action.value == 0x04) {
                        closeSocket();
                    } else {
                        //discard message when value 0x01, 0x02, 0x03
                    }
                    break;
                }
                
                case PayloadType::ROUTINGACTIVATIONREQUEST: {
                    //start routing activation handler with the received message
                    unsigned char result = parseRoutingActivation(data);
                    unsigned char clientAddress [2] = {data[8], data[9]};
                    //clientAddress[0] = data[8];
                    //clientAddress[1] = data[9];
                    
                    unsigned char* message = createRoutingActivationResponse(clientAddress, result);
                    sendMessage(message, _GenericHeaderLength + _ActivationResponseLength);
                    
                    if(result == 0x00 || result == 0x06) {
                        closeSocket();
                    }
                    
                    break;
                }
            }    
        } 
    } 
}

void DoIPServer::receiveUdpMessage(){
    
    unsigned int length = sizeof(clientAdress);
    int readedBytes;
    
    while(true) {
        
        readedBytes = recvfrom(sockfd_receiver, data, _MaxDataSize, 0, (struct sockaddr *) &clientAdress, &length);
        
        if(readedBytes > 0)
        {
             GenericHeaderAction action = parseGenericHeader(data, readedBytes);
             
              switch(action.type) {
                  
                case PayloadType::NEGATIVEACK: {
                    //send NACK
                    unsigned char* message = createGenericHeader(action.type, _NACKLength);
                    message[8] = action.value;
                    sendUdpMessage(message, _GenericHeaderLength + _NACKLength);
                    
                    if(action.value == 0x00 || action.value == 0x04) {
                        closeSocket();
                    } else {
                        //discard message when value 0x01, 0x02, 0x03
                    }
                    break;
                }
                
                 case PayloadType::VEHICLEIDENTREQUEST: {
                      
                     unsigned char* message = createVehicleIdentificationResponse(VIN, LogicalAddress, EID, GID, FurtherActionReq);
                     
                     sendUdpMessage(message, _GenericHeaderLength + _VIResponseLength);
                     closeSocket();
                     
                     break; 
                }
                      
                     
                      
                
              }
             
        }
    }
    
    
}

/**
 * Sends a message back to the connected client
 * @param message   contains generic header and payload specific content
 * @param messageLength 
 */
void DoIPServer::sendMessage(unsigned char* message, int messageLength) {
    write(sockfd_sender, message, messageLength);
}

void DoIPServer::sendUdpMessage(unsigned char* message, int messageLength) {
    sendto(sockfd_receiver, message, messageLength, 0, (struct sockaddr *)&clientAdress, sizeof(clientAdress));
    
}

void DoIPServer::setEIDdefault(){
    
    int fd;
    
    struct ifreq ifr;
    const char* iface = "ens33"; //eth0
    unsigned char* mac;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    ifr.ifr_addr.sa_family = AF_INET;
    
    strncpy((char*)ifr.ifr_name, (const char*)iface, IFNAMSIZ-1);
    
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    
    close(fd);
    
    mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    
    //memcpy(mac, (unsigned char *)ifr.ifr_hwaddr.sa_data, 48);
    
    for(int i = 0; i < 6; i++)
    {
        EID[i] = mac[i];
    }
}

void DoIPServer::setVIN(const char* VINString){
    
    for(int i = 0; i < 18; i++)
    {
        VIN[i] = (unsigned char)VINString[i];
    }
}

void DoIPServer::setLogicalAddress(const unsigned int inputLogAdd){
    LogicalAddress[0] = (inputLogAdd >> 8) & 0xFF;
    LogicalAddress[1] = inputLogAdd & 0xFF;
}

void DoIPServer::setEID(const unsigned long inputEID){
    EID[0] = (inputEID >> 40) &0xFF;
    EID[1] = (inputEID >> 32) &0xFF;
    EID[2] = (inputEID >> 24) &0xFF;
    EID[3] = (inputEID >> 16) &0xFF;
    EID[4] = (inputEID >> 8) &0xFF;
    EID[5] = inputEID  & 0xFF;
}

void DoIPServer::setGID(const unsigned long inputGID){
    GID[0] = (inputGID >> 40) &0xFF;
    GID[1] = (inputGID >> 32) &0xFF;
    GID[2] = (inputGID >> 24) &0xFF;
    GID[3] = (inputGID >> 16) &0xFF;
    GID[4] = (inputGID >> 8) &0xFF;
    GID[5] = inputGID  & 0xFF;
}

void DoIPServer::setFAR(const unsigned int inputFAR){
    FurtherActionReq = inputFAR & 0xFF;
}

