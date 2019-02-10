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
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "VehicleIdentificationHandler.h"
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "DiagnosticMessageHandler.h"

const int _ServerPort = 13400;

const unsigned int _MaxDataSize = 4294967294;


class DoIPServer {

public:
    DoIPServer() = default;
    DoIPServer(DiagnosticCallback diag_callback): diag_callback{diag_callback} { };
    void setCallback(DiagnosticCallback dc, DiagnosticMessageNotification dmn);              
    void setupTcpSocket();
    void listenTcpConnection();
    void setupUdpSocket();
    int receiveMessage();
    int receiveUdpMessage();
    void receiveDiagnosticPayload(unsigned char* address, unsigned char* value, int length);
    void closeSocket();
    void closeUdpSocket();

    void sendDiagnosticAck(bool ackType, unsigned char ackCode);

    void triggerDisconnection();

    int sendNegativeAck(unsigned char ackCode);

    const unsigned char* getData();
    int getDataLength() const;

    void setEIDdefault();
    void setVIN(std::string VINString);
    void setLogicalAddress(const unsigned int inputLogAdd);
    void setEID(const uint64_t inputEID);
    void setGID(const uint64_t inputGID);
    void setFAR(const unsigned int inputFAR);
    
    void setA_DoIP_Announce_Num(int Num);
    
    void setA_DoIP_Announce_Interval(int Interval);
    
    int sendVehicleAnnouncement();


private:
    DiagnosticCallback diag_callback;
    DiagnosticMessageNotification notify_application;
    unsigned char data[_MaxDataSize];
    int dataLength;
    int server_socket_tcp, server_socket_udp, client_socket_tcp;
    struct sockaddr_in serverAddress, clientAddress;
    unsigned char* routedClientAddress;
    
    
    std::string VIN = "00000000000000000";
    unsigned char LogicalAddress [2] = {0x00, 0x00};
    unsigned char EID [6];
    unsigned char GID [6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char FurtherActionReq = 0x00;
    
    int A_DoIP_Announce_Num = 3;    //Default Value = 3
    int A_DoIP_Announce_Interval = 500; //Default Value = 500ms
    
    int broadcast = 1;
    
    int sendMessage(unsigned char* message, int messageLenght);
    int sendUdpMessage(unsigned char* message, int messageLength);
    void setMulticastGroup(const char* address);


};

#endif /* DOIPSERVER_H */
