
#ifndef DOIPCLIENT_H
#define DOIPCLIENT_H
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cstddef>
#include <stdlib.h>
#include <cstdlib>
#include <gtest/gtest.h>

using namespace std;

const int _serverPortNr=13400;
const int _maxDataSize=64;

class DoIPClient{

    unsigned char _receivedData[_maxDataSize];
    int _sockFd;
    int _connected;
    struct sockaddr_in _serverAddr; 
    
    private:
       const pair<int,unsigned char*>* buildRoutingActivationRequest();
    public:
        void startTcpConnection();     
        void sendRoutingActivationRequest();
        void receiveRoutingActivationResponse();
        void closeTcpConnection();
	int getSockFd();
	int getConnected();
};


#endif /* DOIPCLIENT_H */

