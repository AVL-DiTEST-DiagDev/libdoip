#ifndef DOIPSERVER_H
#define DOIPSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/if.h>
#include <unistd.h>
#include <memory>
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "VehicleIdentificationHandler.h"
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "DiagnosticMessageHandler.h"
#include "AliveCheckTimer.h"
#include "DoIPConnection.h"

using CloseConnectionCallback = std::function<void()>;

const int _ServerPort = 13400;
//const unsigned long _MaxDataSize = 4294967294;
//const unsigned long _MaxDataSize = 0xFFFFFF;

class DoIPServer {

public:
    DoIPServer() = default;
    
    void setupTcpSocket();
    std::unique_ptr<DoIPConnection> waitForTcpConnection();
    void setupUdpSocket();
    int receiveUdpMessage();

    void closeTcpSocket();
    void closeUdpSocket();
    
    int sendVehicleAnnouncement();

    void setEIDdefault();
    void setVIN(std::string VINString);
    void setLogicalGatewayAddress(const unsigned short inputLogAdd);
    void setEID(const uint64_t inputEID);
    void setGID(const uint64_t inputGID);
    void setFAR(const unsigned int inputFAR);
    void setA_DoIP_Announce_Num(int Num);
    void setA_DoIP_Announce_Interval(int Interval); 

private:

    int server_socket_tcp, server_socket_udp;
    struct sockaddr_in serverAddress, clientAddress;
    unsigned char data[_MaxDataSize];

    std::string VIN = "00000000000000000";
    unsigned short LogicalGatewayAddress = 0x0000;
    unsigned char EID [6];
    unsigned char GID [6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char FurtherActionReq = 0x00;
    
    int A_DoIP_Announce_Num = 3;    //Default Value = 3
    int A_DoIP_Announce_Interval = 500; //Default Value = 500ms
    
    int broadcast = 1;
    
    int reactToReceivedUdpMessage(int readedBytes);
    
    int sendUdpMessage(unsigned char* message, int messageLength);
    
    void setMulticastGroup(const char* address);
};

#endif /* DOIPSERVER_H */