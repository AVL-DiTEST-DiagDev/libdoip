#ifndef DOIPSERVER_H
#define DOIPSERVER_H

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstddef>
#include <cstring>
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "VehicleIdentificationHandler.h"
#include "DoIPGenericHeaderHandler.h"
#include "RoutingActivationHandler.h"
#include "DiagnosticMessageHandler.h"


using namespace std;

const int _ServerPort = 13400;
const int _MaxDataSize = 64;   

typedef void (*DiagnosticCallback)(unsigned char*, int);

class DoIPServer {
  
        char VIN [18] = "00000000000000000"; //17 chars VIN + /0 char
        unsigned char LogicalAddress [2] = {0x00, 0x00};
        unsigned char EID [6];
        unsigned char GID [6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char FurtherActionReq = 0x00;

    public:
	DoIPServer(DiagnosticCallback diag_callback): diag_callback{diag_callback} { };
	void receiveDiagnosticPayload(unsigned char* value, int length);
	const DiagnosticCallback diag_callback;

        unsigned char data[_MaxDataSize];
        int sockfd_receiver, sockfd_sender;

        struct sockaddr_in serverAdress, clientAdress;
	      unsigned char* routedClientAddress;
        void setupSocket();
        void setupUdpSocket();
        void receiveMessage();
        void receiveUdpMessage();
        void sendMessage(unsigned char* message, int messageLenght);
        void sendUdpMessage(unsigned char* message, int messageLength);
        void closeSocket();
        void setEIDdefault();
        void setVIN(const char* VINString);
        void setLogicalAddress(const unsigned int inputLogAdd);
        void setEID(const unsigned long inputEID);
        void setGID(const unsigned long inputGID);
        void setFAR(const unsigned int inputFAR);
        
   
        
};

        








#endif /* DOIPSERVER_H */

