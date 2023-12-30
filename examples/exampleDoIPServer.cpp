#include "DoIPServer.h"

#include<iostream>
#include<iomanip>
#include<thread>

using namespace std;

static const unsigned short LOGICAL_ADDRESS = 0x28;

DoIPServer server;
unique_ptr<DoIPConnection> connection(nullptr);
std::vector<std::thread> doipReceiver;
bool serverActive = false;

/**
 * Is called when the doip library receives a diagnostic message.
 * @param address   logical address of the ecu
 * @param data      message which was received
 * @param length    length of the message
 */
void ReceiveFromLibrary(unsigned short address, unsigned char* data, int length) {
    cout << "DoIP Message received from 0x" << hex << address << ": ";
    for(int i = 0; i < length; i++) {
        cout << hex << setw(2) << (int)data[i] << " ";
    }
    cout << endl;

    if(length > 2 && data[0] == 0x22)  {
        cout << "-> Send diagnostic message positive response" << endl;
        unsigned char responseData[] = { 0x62, data[1], data[2], 0x01, 0x02, 0x03, 0x04};
        connection->sendDiagnosticPayload(LOGICAL_ADDRESS, responseData, sizeof(responseData));
    } else {
        cout << "-> Send diagnostic message negative response" << endl;
        unsigned char responseData[] = { 0x7F, data[0], 0x11};
        connection->sendDiagnosticPayload(LOGICAL_ADDRESS, responseData, sizeof(responseData));
    }


}

/**
 * Will be called when the doip library receives a diagnostic message.
 * The library notifies the application about the message.
 * Checks if there is a ecu with the logical address
 * @param targetAddress     logical address to the ecu
 * @return                  If a positive or negative ACK should be send to the client
 */
bool DiagnosticMessageReceived(unsigned short targetAddress) {
    (void)targetAddress;
    unsigned char ackCode;

    cout << "Received Diagnostic message" << endl;

    //send positiv ack
    ackCode = 0x00;
    cout << "-> Send positive diagnostic message ack" << endl;
    connection->sendDiagnosticAck(LOGICAL_ADDRESS, true, ackCode);

    return true;
}

/**
 * Closes the connection of the server by ending the listener threads
 */
void CloseConnection() {
    cout << "Connection closed" << endl;
    //serverActive = false;
}

/*
 * Check permantly if udp message was received
 */
void listenUdp() {

    while(serverActive) {
        server.receiveUdpMessage();
    }
}

/*
 * Check permantly if tcp message was received
 */
void listenTcp() {

    server.setupTcpSocket();

    while(true) {
        connection = server.waitForTcpConnection();
        connection->setCallback(ReceiveFromLibrary, DiagnosticMessageReceived, CloseConnection);
        connection->setGeneralInactivityTime(50000);

         while(connection->isSocketActive()) {
             connection->receiveTcpMessage();
         }
    }
}

void ConfigureDoipServer() {
    // VIN needs to have a fixed length of 17 bytes.
    // Shorter VINs will be padded with '0'
    server.setVIN("FOOBAR");
    server.setLogicalGatewayAddress(LOGICAL_ADDRESS);
    server.setGID(0);
    server.setFAR(0);
    server.setEID(0);

    // doipserver->setA_DoIP_Announce_Num(tempNum);
    // doipserver->setA_DoIP_Announce_Interval(tempInterval);

}

int main() {
    ConfigureDoipServer();

    server.setupUdpSocket();

    serverActive = true;
    doipReceiver.push_back(thread(&listenUdp));
    doipReceiver.push_back(thread(&listenTcp));

    server.sendVehicleAnnouncement();

    doipReceiver.at(0).join();
    doipReceiver.at(1).join();
    return 0;
}
