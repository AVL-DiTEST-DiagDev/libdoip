
#ifndef DOIPCLIENT_H
#define DOIPCLIENT_H
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstddef>
#include <stdlib.h>
#include <cstdlib>

#include "DiagnosticMessageHandler.h"

using namespace std;

const int _serverPortNr=13400;
const int _maxDataSize=64;


class DoIPClient{

    unsigned char _receivedData[_maxDataSize];
    int _sockFd;
    int _connected;
    struct sockaddr_in _serverAddr; 
    
    unsigned char VINResult [17];
    unsigned char LogicalAddressResult [2];
    unsigned char EIDResult [6];
    unsigned char GIDResult [6];
    unsigned char FurtherActionReqResult;
    
    private:
       const pair<int, unsigned char*>* buildRoutingActivationRequest();
       const pair<int, unsigned char*>* buildVehicleIdentificationRequest();
       void parseVIResponseInformation(unsigned char* data);
       
    public:
        void startTcpConnection();   
        void startUdpConnection();
        void sendRoutingActivationRequest();
        void sendVehicleIdentificationRequest(const char* address);
        void receiveRoutingActivationResponse();
        void receiveUdpMessage();
        void receiveMessage();
	void sendDiagnosticMessage(unsigned char* userData, int userDataLength);
        void closeTcpConnection();
        void closeUdpConnection();
  
	int getSockFd();
	int getConnected();
};



#endif /* DOIPCLIENT_H */

