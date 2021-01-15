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
#include <climits>
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "VehicleIdentificationHandler.h"
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "DiagnosticMessageHandler.h"
#include "AliveCheckTimer.h"

using CloseConnectionCallback = std::function<void()>;

const int _ServerPort = 13400;
const unsigned int _MaxDataSize = 0xFFFFU;

class DoIPServer {

public:
    DoIPServer() = default;
    DoIPServer(DiagnosticCallback diag_callback): diag_callback{diag_callback} { };
    
    void setupTcpSocket();
    void listenTcpConnection();
    void setupUdpSocket();
    int receiveTcpMessage();
    int receiveUdpMessage();
    void receiveDiagnosticPayload(unsigned char* address, unsigned char* data, int length);
    void closeSocket();
    void closeUdpSocket();
    void triggerDisconnection();
    
    void sendDiagnosticAck(bool ackType, unsigned char ackCode);
    int sendNegativeAck(unsigned char ackCode);
    int sendVehicleAnnouncement();

    const unsigned char* getData();
    int getDataLength() const;

    void setCallback(DiagnosticCallback dc, DiagnosticMessageNotification dmn, CloseConnectionCallback ccb);                       
    void setEIDdefault();
    void setVIN(std::string VINString);
    void setLogicalAddress(const unsigned int inputLogAdd);
    void setEID(const uint64_t inputEID);
    void setGID(const uint64_t inputGID);
    void setFAR(const unsigned int inputFAR);
    void setGeneralInactivityTime(const uint16_t seconds);   
    void setA_DoIP_Announce_Num(int Num);
    void setA_DoIP_Announce_Interval(int Interval); 

private:
    AliveCheckTimer aliveCheckTimer;
    DiagnosticCallback diag_callback;
    CloseConnectionCallback close_connection;
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
    
    int reactToReceivedTcpMessage(int readedBytes);
    int reactToReceivedUdpMessage(int readedBytes);
    
    int sendMessage(unsigned char* message, int messageLenght);
    int sendUdpMessage(unsigned char* message, int messageLength);
    
    void setMulticastGroup(const char* address);
    void aliveCheckTimeout();
};

#endif /* DOIPSERVER_H */