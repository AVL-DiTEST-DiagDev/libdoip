
#ifndef DOIPCLIENT_H
#define DOIPCLIENT_H
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstddef>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>

#include "DiagnosticMessageHandler.h"
#include "DoIPGenericHeaderHandler.h"

const int _serverPortNr=13400;
const int _maxDataSize=64;


class DoIPClient{
    
public:
    void startTcpConnection();   
    void startUdpConnection();
    void sendRoutingActivationRequest();
    void sendVehicleIdentificationRequest(const char* address);
    void receiveRoutingActivationResponse();
    void receiveUdpMessage();
    void receiveMessage();
    void sendDiagnosticMessage(unsigned char* targetAddress, unsigned char* userData, int userDataLength);
    void sendAliveCheckResponse();
    void setSourceAddress(unsigned char* address);
    void displayVIResponseInformation();
    void closeTcpConnection();
    void closeUdpConnection();
    void reconnectServer();

    int getSockFd();
    int getConnected();
    
private:
    unsigned char _receivedData[_maxDataSize];
    int _sockFd, _sockFd_udp, _connected;
    int broadcast = 1;
    struct sockaddr_in _serverAddr, _clientAddr; 
    unsigned char sourceAddress [2];
    
    unsigned char VINResult [17];
    unsigned char LogicalAddressResult [2];
    unsigned char EIDResult [6];
    unsigned char GIDResult [6];
    unsigned char FurtherActionReqResult;
    
    const std::pair<int, unsigned char*>* buildRoutingActivationRequest();
    const std::pair<int, unsigned char*>* buildVehicleIdentificationRequest();
    void parseVIResponseInformation(unsigned char* data);
    
    int emptyMessageCounter = 0;
};



#endif /* DOIPCLIENT_H */

