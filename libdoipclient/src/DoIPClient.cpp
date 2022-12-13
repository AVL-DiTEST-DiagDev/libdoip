#include "DoIPClient_h.h"

/*
 *Set up the connection between client and server
 */
void DoIPClient::startTcpConnection() {

    const char* ipAddr = "127.0.0.1";
    bool connectedFlag = false;
    _sockFd = socket(AF_INET,SOCK_STREAM,0);   
    
    if(_sockFd>=0)
    {
        std::cout << "Client TCP-Socket created successfully" << std::endl;

        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = htons(_serverPortNr);
        inet_aton(ipAddr,&(_serverAddr.sin_addr)); 
        
        while(!connectedFlag)
        {
            _connected = connect(_sockFd,(struct sockaddr *) &_serverAddr,sizeof(_serverAddr));
            if(_connected!=-1)
            {
                connectedFlag = true;
                std::cout << "Connection to server established" << std::endl;
            }
        }  
    }   
}

void DoIPClient::startUdpConnection(){
    
    _sockFd_udp = socket(AF_INET,SOCK_DGRAM, 0); 
    
    if(_sockFd_udp >= 0)
    {
        std::cout << "Client-UDP-Socket created successfully" << std::endl;
        
        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = htons(_serverPortNr);
        _serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        _clientAddr.sin_family = AF_INET;
        _clientAddr.sin_port = htons(_serverPortNr);
        _clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        //binds the socket to any IP Address and the Port Number 13400
        bind(_sockFd_udp, (struct sockaddr *)&_clientAddr, sizeof(_clientAddr));
    }
}

/*
 * closes the client-socket
 */
void DoIPClient::closeTcpConnection(){  
    close(_sockFd); 
}

void DoIPClient::closeUdpConnection(){
    close(_sockFd_udp);
}

void DoIPClient::reconnectServer(){
    closeTcpConnection();
    startTcpConnection();
}

/*
 *Build the Routing-Activation-Request for server
 */
const std::pair<int,unsigned char*>* DoIPClient::buildRoutingActivationRequest() {
    
   std::pair <int,unsigned char*>* rareqWithLength= new std::pair<int,unsigned char*>();
   int rareqLength=15;
   unsigned char * rareq= new unsigned char[rareqLength];
  
   //Generic Header
   rareq[0]=0x02;  //Protocol Version
   rareq[1]=0xFD;  //Inverse Protocol Version
   rareq[2]=0x00;  //Payload-Type
   rareq[3]=0x05;
   rareq[4]=0x00;  //Payload-Length
   rareq[5]=0x00;  
   rareq[6]=0x00;
   rareq[7]=0x07;
   
   //Payload-Type specific message-content
   rareq[8]=0x0E;  //Source Address
   rareq[9]=0x00;
   rareq[10]=0x00; //Activation-Type
   rareq[11]=0x00; //Reserved ISO(default)
   rareq[12]=0x00;
   rareq[13]=0x00;
   rareq[14]=0x00;
   
   rareqWithLength->first=rareqLength;
   rareqWithLength->second=rareq;
  
   return rareqWithLength;
}

/*
 * Send the builded request over the tcp-connection to server
 */
void DoIPClient::sendRoutingActivationRequest() {
        
    const std::pair <int,unsigned char*>* rareqWithLength=buildRoutingActivationRequest();
    write(_sockFd,rareqWithLength->second,rareqWithLength->first);    
}

/**
 * Sends a diagnostic message to the server
 * @param targetAddress     the address of the ecu which should receive the message
 * @param userData          data that will be given to the ecu
 * @param userDataLength    length of userData
 */
void DoIPClient::sendDiagnosticMessage(unsigned char* targetAddress, unsigned char* userData, int userDataLength) {
    unsigned short sourceAddress = 0x0E00;
    unsigned char* message = createDiagnosticMessage(sourceAddress, targetAddress, userData, userDataLength);

    write(_sockFd, message, _GenericHeaderLength + _DiagnosticMessageMinimumLength + userDataLength);
}

/**
 * Sends a alive check response containing the clients source address to the server
 */
void DoIPClient::sendAliveCheckResponse() {
    int responseLength = 2;
    unsigned char* message = createGenericHeader(PayloadType::ALIVECHECKRESPONSE, responseLength);
    message[8] = sourceAddress[0];
    message[9] = sourceAddress[1];
    write(_sockFd, message, _GenericHeaderLength + responseLength);
}

/*
 * Receive a message from server
 */
void DoIPClient::receiveMessage() {
    
    int readedBytes = recv(_sockFd,_receivedData,_maxDataSize, 0);
    
    if(!readedBytes) //if server is disconnected from client; client gets empty messages
    {
        emptyMessageCounter++;
        
        if(emptyMessageCounter == 5)
        {
            std::cout << "Received to many empty messages. Reconnect TCP connection" << std::endl;
            emptyMessageCounter = 0;
            reconnectServer();
        }
        return;
    }
	
    printf("Client received: ");
    for(int i = 0; i < readedBytes; i++)
    {
        printf("0x%02X ", _receivedData[i]);
    }    
    printf("\n ");	
    
    GenericHeaderAction action = parseGenericHeader(_receivedData, readedBytes);

    if(action.type == PayloadType::DIAGNOSTICPOSITIVEACK || action.type == PayloadType::DIAGNOSTICNEGATIVEACK) {
        switch(action.type) {
            case PayloadType::DIAGNOSTICPOSITIVEACK: {
                std::cout << "Client received diagnostic message positive ack with code: ";
                printf("0x%02X ", _receivedData[12]);
                break;
            }
            case PayloadType::DIAGNOSTICNEGATIVEACK: {
                std::cout << "Client received diagnostic message negative ack with code: ";
                printf("0x%02X ", _receivedData[12]);
                break;
            }
            default: {
                std::cerr << "not handled payload type occured in receiveMessage()" << std::endl;
                break;
            }
        }
        std::cout << std::endl;
    }
 
}

void DoIPClient::receiveUdpMessage() {
    
    unsigned int length = sizeof(_clientAddr);
    
    int readedBytes;
    readedBytes = recvfrom(_sockFd_udp, _receivedData, _maxDataSize, 0, (struct sockaddr*)&_clientAddr, &length);
    
    if(PayloadType::VEHICLEIDENTRESPONSE == parseGenericHeader(_receivedData, readedBytes).type)
    {
        parseVIResponseInformation(_receivedData);
    }
    
}

const std::pair<int,unsigned char*>* DoIPClient::buildVehicleIdentificationRequest(){
    
    std::pair <int,unsigned char*>* rareqWithLength= new std::pair<int,unsigned char*>();
    int rareqLength= 8;
    unsigned char * rareq= new unsigned char[rareqLength];

    //Generic Header
    rareq[0]=0x02;  //Protocol Version
    rareq[1]=0xFD;  //Inverse Protocol Version
    rareq[2]=0x00;  //Payload-Type
    rareq[3]=0x01;
    rareq[4]=0x00;  //Payload-Length
    rareq[5]=0x00;  
    rareq[6]=0x00;
    rareq[7]=0x00;

    rareqWithLength->first=rareqLength;
    rareqWithLength->second=rareq;

    return rareqWithLength;
    
}

void DoIPClient::sendVehicleIdentificationRequest(const char* address){
     
    int setAddressError = inet_aton(address,&(_serverAddr.sin_addr));
    
    if(setAddressError != 0)
    {
        std::cout <<"Address set succesfully"<<std::endl;
    }
    else
    {
        std::cout << "Could not set Address. Try again" << std::endl;
    }
    
    int socketError = setsockopt(_sockFd_udp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) );
         
    if(socketError == 0)
    {
        std::cout << "Broadcast Option set successfully" << std::endl;
    }
      
    const std::pair <int,unsigned char*>* rareqWithLength=buildVehicleIdentificationRequest();
    
    int sendError = sendto(_sockFd_udp, rareqWithLength->second,rareqWithLength->first, 0, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr));
    
    if(sendError > 0)
    {
        std::cout << "Sending Vehicle Identification Request" << std::endl;
    }
}

/**
 * Sets the source address for this client
 * @param address   source address for the client
 */
void DoIPClient::setSourceAddress(unsigned char* address) {
    sourceAddress[0] = address[0];
    sourceAddress[1] = address[1];
}

/*
* Getter for _sockFD
*/
int DoIPClient::getSockFd() {
    return _sockFd;
}

/*
* Getter for _connected
*/
int DoIPClient::getConnected() {
    return _connected;
}

void DoIPClient::parseVIResponseInformation(unsigned char* data){
    
    //VIN
    int j = 0;
    for(int i = 8; i <= 24; i++)
    {      
        VINResult[j] = data[i];
        j++;
    }
    
    //Logical Adress
    j = 0;
    for(int i = 25; i <= 26; i++)
    {
        LogicalAddressResult[j] = data[i];
        j++;
    }
      
    //EID
    j = 0;
    for(int i = 27; i <= 32; i++)
    {
        EIDResult[j] = data[i];
        j++;
    }
    
    //GID
    j = 0;
    for(int i = 33; i <= 38; i++)
    {
        GIDResult[j] = data[i];
        j++;
    }
    
    //FurtherActionRequest
    FurtherActionReqResult = data[39];
    
}

void DoIPClient::displayVIResponseInformation()
{
    //output VIN
    std::cout << "VIN: ";
    for(int i = 0; i < 17 ;i++)
    {
        std::cout << (unsigned char)(int)VINResult[i];
    }
    std::cout << std::endl;
    
    //output LogicalAddress
    std::cout << "LogicalAddress: ";
    for(int i = 0; i < 2; i++)
    {
        printf("%02X", (int)LogicalAddressResult[i]);
    }
    std::cout << std::endl;
    
    //output EID
    std::cout << "EID: ";
    for(int i = 0; i < 6; i++)
    {
        printf("%02X", EIDResult[i]);
    }
    std::cout << std::endl;
    
     //output GID
    std::cout << "GID: ";
    for(int i = 0; i < 6; i++)
    {
        printf("%02X", (int)GIDResult[i]);
    }
    std::cout << std::endl;
    
    //output FurtherActionRequest
    std::cout << "FurtherActionRequest: ";
    printf("%02X", (int)FurtherActionReqResult);
    
    std::cout << std::endl;
}
