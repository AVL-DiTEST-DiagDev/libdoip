#include "DoIPServer.h"

/*
 * Set up a socket till the point where the server waits till a client
 * approaches to make a connection
 */
void DoIPServer::setupSocket() {
    
    sockfd_receiver = socket(AF_INET, SOCK_STREAM, 0);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(_ServerPort);
    
    //binds the socket to the address and port number
    bind(sockfd_receiver, (struct sockaddr *)&serverAddress, sizeof(serverAddress));     
    //waits till client approach to make connection
    listen(sockfd_receiver, 5);                                                          
    
    sockfd_sender = accept(sockfd_receiver, (struct sockaddr*) NULL, NULL);
}

void DoIPServer::setupUdpSocket(){
    
    sockfd_receiver_udp = socket(AF_INET, SOCK_DGRAM, 0);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(_ServerPort);
    
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr=htonl(INADDR_ANY);
    clientAddress.sin_port=htons(_ServerPort);
    
    if(sockfd_receiver_udp < 0)
        std::cout << "Error setting up a udp socket" << std::endl;
    
    //binds the socket to any IP Address and the Port Number 13400
    bind(sockfd_receiver_udp, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); 
    
    //setting the IP Address for Multicast
    setMulticastGroup("224.0.0.2");
    
}

/*
 * Closes the socket for this server
 */
void DoIPServer::closeSocket() {
    close(sockfd_receiver);
    close(sockfd_sender);
}

void DoIPServer::closeUdpSocket() {
    close(sockfd_receiver_udp);
}


void DoIPServer::triggerDisconnection() {
    
    bool socketsClosed = false;
    
    std::cout << "Disconnecting Client from Server" << std::endl;
    
    while(socketsClosed == false)
    {
        int tcpSenderClosed = close(sockfd_sender);
        
        if(tcpSenderClosed == 0)
        {
            socketsClosed = true;
            
            std::cout << "Connecting to the Client" << std::endl;
            
            sockfd_sender = accept(sockfd_receiver, (struct sockaddr*) NULL, NULL);      
        }
        else
        {
            std::cout << "Disconnecting failed. Try Again" << std::endl;
        }
    }
    
}

/*
 * Receives a message from the client and determine how to process the message
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPServer::receiveMessage() {

    int readedBytes = recv(sockfd_sender, data, _MaxDataSize, 0);

    if(readedBytes > 0) {
        dataLength = readedBytes;
        GenericHeaderAction action = parseGenericHeader(data, readedBytes);
       
        int sendedBytes;
        switch(action.type) {
            case PayloadType::NEGATIVEACK: {
                //send NACK
                sendedBytes = sendNegativeAck(action.value);
                
                if(action.value == 0x00 || action.value == 0x04) {
                    closeSocket();
                    return -1;
                }
                
                return sendedBytes;
            }
            
            case PayloadType::ROUTINGACTIVATIONREQUEST: {
                //start routing activation handler with the received message
                unsigned char result = parseRoutingActivation(data);
                unsigned char clientAddress [2] = {data[8], data[9]};
                
                unsigned char* message = createRoutingActivationResponse(clientAddress, result);
                sendedBytes = sendMessage(message, _GenericHeaderLength + _ActivationResponseLength);
                
                if(result == 0x00 || result == 0x06) {
                    closeSocket();
                    return -1;
                } else {
                    //Routing Activation Request was successfull, save address of the client
                    routedClientAddress = new unsigned char[2];
                    routedClientAddress[0] = data[8];
                    routedClientAddress[1] = data[9];
                }

                return sendedBytes;
            }
				
            case PayloadType::DIAGNOSTICMESSAGE: {
                              
                unsigned char target_address [2] = {data[10], data[11]};           
                bool ack = diag_notification(target_address);
                
                if(ack)
                    parseDiagnosticMessage(diag_callback, routedClientAddress, data, readedBytes - _GenericHeaderLength);
                
                break;
            }
            
            default: {
                std::cerr << "not handled payload type occured in receiveMessage()" << std::endl;
                return -1;
            }
        }  
    }
    
    return -1;
}

/*
 * Receives a udp message and determine how to process the message
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPServer::receiveUdpMessage(){
    
    unsigned int length = sizeof(clientAddress);   
    int readedBytes = recvfrom(sockfd_receiver_udp, data, _MaxDataSize, 0, (struct sockaddr *) &clientAddress, &length);
        
    if(readedBytes > 0) {
        dataLength = readedBytes;
        GenericHeaderAction action = parseGenericHeader(data, readedBytes);

        int sendedBytes;
        switch(action.type) {

            case PayloadType::VEHICLEIDENTRESPONSE:{    //server should not send a negative ACK if he receives the sended VehicleIdentificationAnnouncement
                
                return -1;
            }
            
            case PayloadType::NEGATIVEACK: {
                //send NACK
                unsigned char* message = createGenericHeader(action.type, _NACKLength);
                message[8] = action.value;
                sendedBytes = sendUdpMessage(message, _GenericHeaderLength + _NACKLength);

                if(action.value == 0x00 || action.value == 0x04) {
                    closeSocket();
                    return -1;
                } else {
                    //discard message when value 0x01, 0x02, 0x03
                }
                return sendedBytes;
            }

            case PayloadType::VEHICLEIDENTREQUEST: {
                unsigned char* message = createVehicleIdentificationResponse(VIN, LogicalAddress, EID, GID, FurtherActionReq);
                sendedBytes = sendUdpMessage(message, _GenericHeaderLength + _VIResponseLength);   
                
                return sendedBytes;
            }

            default: { 
                std::cerr << "not handled payload type occured in receiveUdpMessage()" << std::endl;
                return -1;
            }
        }
    }
    
    return -1;
}

/**
 * Sends a message back to the connected client
 * @param message           contains generic header and payload specific content
 * @param messageLength     length of the complete message
 * @return                  number of bytes written is returned,
 *                          or -1 if error occurred
 */
int DoIPServer::sendMessage(unsigned char* message, int messageLength) {
    int result = write(sockfd_sender, message, messageLength);
    return result;
}


int DoIPServer::sendUdpMessage(unsigned char* message, int messageLength)  { //sendUdpMessage after receiving a message from the client
    
    int result = sendto(sockfd_receiver_udp, message, messageLength, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
    
    return result;
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

void DoIPServer::setVIN( std::string VINString){
    
    VIN = VINString;
}

void DoIPServer::setLogicalAddress(const unsigned int inputLogAdd){
    LogicalAddress[0] = (inputLogAdd >> 8) & 0xFF;
    LogicalAddress[1] = inputLogAdd & 0xFF;
}

void DoIPServer::setEID(const uint64_t inputEID){
    EID[0] = (inputEID >> 40) &0xFF;
    EID[1] = (inputEID >> 32) &0xFF;
    EID[2] = (inputEID >> 24) &0xFF;
    EID[3] = (inputEID >> 16) &0xFF;
    EID[4] = (inputEID >> 8) &0xFF;
    EID[5] = inputEID  & 0xFF;
}

void DoIPServer::setGID(const uint64_t inputGID){
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

void DoIPServer::setA_DoIP_Announce_Num(int Num){
    A_DoIP_Announce_Num = Num;
}

void DoIPServer::setA_DoIP_Announce_Interval(int Interval){
    A_DoIP_Announce_Interval = Interval;
}



/*
 * Receive diagnostic message payload from the server application, which will be sended back to the client
 * @param value     received payload
 * @param length    length of received payload
 */
void DoIPServer::receiveDiagnosticPayload(unsigned char* address, unsigned char* value, int length) {

    printf("DoiPServer received from server application: ");
    for(int i = 0; i < length; i++) {
        printf("0x%x ", value[i]);    
    }
    printf("\n");
    
    unsigned char* message = createDiagnosticMessage(address, routedClientAddress, value, length);  
    sendMessage(message, _GenericHeaderLength + _DiagnosticMessageMinimumLength + length);
}

/*
 * Getter to the last received data
 * @return  pointer to the received data array
 */
const unsigned char* DoIPServer::getData() {
    return data;
}

/*
 * Getter to the length of the last received data
 * @return  length of received data
 */
int DoIPServer::getDataLength() const {
    return dataLength;
}

void DoIPServer::setMulticastGroup(const char* address) {
    
    int loop = 1;
    
    //set Option using the same Port for multiple Sockets
    int setPort = setsockopt(sockfd_receiver_udp, SOL_SOCKET, SO_REUSEADDR, &loop, sizeof(loop));
    
    if(setPort < 0)
    {
        std::cout << "Setting Port Error" << std::endl;
    }
    
     
    struct ip_mreq mreq;
    
    mreq.imr_multiaddr.s_addr = inet_addr(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    //set Option to join Multicast Group
    int setGroup = setsockopt(sockfd_receiver_udp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
    
    if(setGroup < 0)
    {
        std::cout <<"Setting Address Error" << std::endl;
    }
}

/*
 * Set the callback function for this doip server instance
 * @cb  callback function
 */
void DoIPServer::setCallback(DiagnosticCallback dc, DiagnosticMessageNotification dmn) {
    diag_callback = dc;
    diag_notification = dmn;
}

void DoIPServer::sendDiagnosticAck(PayloadType type, unsigned char ackCode) {
    unsigned char data_TA [2] = { 0x0E, 0x00 };
    unsigned char data_SA [2] = { 0xE0, 0x00 };
    
    unsigned char* message = createDiagnosticACK(type, data_SA, data_TA, ackCode);
    sendMessage(message, _GenericHeaderLength + _DiagnosticPositiveACKLength);
}

/**
 * Prepares a generic header nack and sends it to the client
 * @param ackCode       NACK-Code which will be included in the message
 * @return              amount of bytes sended to the client
 */
int DoIPServer::sendNegativeAck(unsigned char ackCode) {
    unsigned char* message = createGenericHeader(PayloadType::NEGATIVEACK, _NACKLength);
    message[8] = ackCode;
    int sendedBytes = sendMessage(message, _GenericHeaderLength + _NACKLength);
    return sendedBytes;
}

int DoIPServer::sendVehicleAnnouncement() {
    
    const char* address = "255.255.255.255";
    
    int setAddressError = inet_aton(address,&(clientAddress.sin_addr));
    
    if(setAddressError != 0)
    {
        std::cout <<"Broadcast Address set succesfully"<<std::endl;
    }
    
    int socketError = setsockopt(sockfd_receiver_udp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) );
         
    if(socketError == 0)
    {
        std::cout << "Broadcast Option set successfully" << std::endl;
    }
    
    int sendedmessage;
    
    unsigned char* message = createVehicleIdentificationResponse(VIN, LogicalAddress, EID, GID, FurtherActionReq);
    
    for(int i = 0; i < A_DoIP_Announce_Num; i++)
    {
        
        sendedmessage = sendto(sockfd_receiver_udp, message, _GenericHeaderLength + _VIResponseLength, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        
        if(sendedmessage > 0)
        {
            std::cout<<"Sending Vehicle Announcement"<<std::endl;
        }
        else
        {
            std::cout<<"Failed Sending Vehicle Announcement"<<std::endl;
        }   
        usleep(A_DoIP_Announce_Interval*1000);
        
    }
    return sendedmessage;
    
}
