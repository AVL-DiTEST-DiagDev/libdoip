#include "DoIPServer.h"

/*
 * Set up a tcp socket, so the socket is ready to accept a connection 
 */
void DoIPServer::setupTcpSocket() {

    server_socket_tcp = socket(AF_INET, SOCK_STREAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(_ServerPort);
    
    //binds the socket to the address and port number
    bind(server_socket_tcp, (struct sockaddr *)&serverAddress, sizeof(serverAddress));     
}

/*
 *  Wait till a client attempts a connection and accepts it
 */
std::unique_ptr<DoIPConnection> DoIPServer::waitForTcpConnection() {
    //waits till client approach to make connection
    listen(server_socket_tcp, 5);                                                          
    int tcpSocket = accept(server_socket_tcp, (struct sockaddr*) NULL, NULL);
    return std::unique_ptr<DoIPConnection>(new DoIPConnection(tcpSocket, LogicalGatewayAddress));
}

void DoIPServer::setupUdpSocket() {
    
    server_socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(_ServerPort);
    
    if(server_socket_udp < 0)
        std::cout << "Error setting up a udp socket" << std::endl;
    
    //binds the socket to any IP Address and the Port Number 13400
    bind(server_socket_udp, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); 
    
    //setting the IP Address for Multicast
    setMulticastGroup("224.0.0.2");
}

/*
 * Closes the socket for this server
 */
void DoIPServer::closeTcpSocket() {
    close(server_socket_tcp);
}

void DoIPServer::closeUdpSocket() {
    close(server_socket_udp);
}

/*
 * Receives a udp message and calls reactToReceivedUdpMessage method
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPServer::receiveUdpMessage(){
    
    unsigned int length = sizeof(serverAddress);   
    int readBytes = recvfrom(server_socket_udp, data, _MaxDataSize, 0, (struct sockaddr *) &serverAddress, &length);
        
    int sentBytes = reactToReceivedUdpMessage(readBytes);
    
    return sentBytes;
}


/*
 * Receives a udp message and determine how to process the message
 * @return      amount of bytes which were send back to client
 *              or -1 if error occurred     
 */
int DoIPServer::reactToReceivedUdpMessage(int readedBytes) {
        
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

            if(action.value == _IncorrectPatternFormatCode || 
                    action.value == _InvalidPayloadLengthCode) {
                return -1;
            } else {
                //discard message when value 0x01, 0x02, 0x03
            }
            return sendedBytes;
        }

        case PayloadType::VEHICLEIDENTREQUEST: {
            unsigned char* message = createVehicleIdentificationResponse(VIN, LogicalGatewayAddress, EID, GID, FurtherActionReq);
            sendedBytes = sendUdpMessage(message, _GenericHeaderLength + _VIResponseLength);   

            return sendedBytes;
        }

        default: { 
            std::cerr << "not handled payload type occured in receiveUdpMessage()" << std::endl;
            return -1;
        }
    }   
    return -1;
}

int DoIPServer::sendUdpMessage(unsigned char* message, int messageLength)  { //sendUdpMessage after receiving a message from the client
    //if the server receives a message from a client, than the response should be send back to the client address and port
    clientAddress.sin_port = serverAddress.sin_port;
    clientAddress.sin_addr.s_addr = serverAddress.sin_addr.s_addr;
    
    int result = sendto(server_socket_udp, message, messageLength, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
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

void DoIPServer::setLogicalGatewayAddress(const unsigned short inputLogAdd){
    LogicalGatewayAddress = inputLogAdd;
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



void DoIPServer::setMulticastGroup(const char* address) {
    
    int loop = 1;
    
    //set Option using the same Port for multiple Sockets
    int setPort = setsockopt(server_socket_udp, SOL_SOCKET, SO_REUSEADDR, &loop, sizeof(loop));
    
    if(setPort < 0)
    {
        std::cout << "Setting Port Error" << std::endl;
    }
    
     
    struct ip_mreq mreq;
    
    mreq.imr_multiaddr.s_addr = inet_addr(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    //set Option to join Multicast Group
    int setGroup = setsockopt(server_socket_udp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
    
    if(setGroup < 0)
    {
        std::cout <<"Setting Address Error" << std::endl;
    }
}


int DoIPServer::sendVehicleAnnouncement() {
    
    const char* address = "255.255.255.255";
    
    //setting the destination port for the Announcement to 13401
    clientAddress.sin_port=htons(13401);
    
    int setAddressError = inet_aton(address,&(clientAddress.sin_addr));
    
    
    if(setAddressError != 0)
    {
        std::cout <<"Broadcast Address set succesfully"<<std::endl;
    }
    
    int socketError = setsockopt(server_socket_udp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) );
         
    if(socketError == 0)
    {
        std::cout << "Broadcast Option set successfully" << std::endl;
    }
    
    int sendedmessage;
    
    unsigned char* message = createVehicleIdentificationResponse(VIN, LogicalGatewayAddress, EID, GID, FurtherActionReq);
    
    for(int i = 0; i < A_DoIP_Announce_Num; i++)
    {
        
        sendedmessage = sendto(server_socket_udp, message, _GenericHeaderLength + _VIResponseLength, 0, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
        
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
