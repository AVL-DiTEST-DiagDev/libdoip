#ifndef DOIPCONNECTION_H
#define DOIPCONNECTION_H

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
#include "AliveCheckTimer.h"

using CloseConnectionCallback = std::function<void()>;

const unsigned long _MaxDataSize = 0xFFFFFF;

class DoIPConnection {

public:
    DoIPConnection(int tcpSocket, unsigned short logicalGatewayAddress): 
        tcpSocket(tcpSocket), logicalGatewayAddress(logicalGatewayAddress) { };
    
    int receiveTcpMessage();
    void sendDiagnosticPayload(unsigned short sourceAddress, unsigned char* data, int length);
    bool isSocketActive() { return tcpSocket != 0; };

    void triggerDisconnection();
    
    void sendDiagnosticAck(unsigned short sourceAddress, bool ackType, unsigned char ackCode);
    int sendNegativeAck(unsigned char ackCode);

    const unsigned char* getData();
    int getDataLength() const;

    void setCallback(DiagnosticCallback dc, DiagnosticMessageNotification dmn, CloseConnectionCallback ccb);                       
    void setGeneralInactivityTime(const uint16_t seconds);   

private:

    int tcpSocket;

    AliveCheckTimer aliveCheckTimer;
    DiagnosticCallback diag_callback;
    CloseConnectionCallback close_connection;
    DiagnosticMessageNotification notify_application;

    unsigned char data[_MaxDataSize];
    int dataLength;
    unsigned char* routedClientAddress;
    unsigned short logicalGatewayAddress = 0x0000;
        
    void closeSocket();

    int reactToReceivedTcpMessage(int readedBytes);
    
    int sendMessage(unsigned char* message, int messageLenght);
    
    void aliveCheckTimeout();
};

#endif /* DOIPCONNECTION_H */