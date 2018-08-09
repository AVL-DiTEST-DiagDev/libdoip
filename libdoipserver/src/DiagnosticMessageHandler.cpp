#include "DiagnosticMessageHandler.h"

/**
 * Checks if a received Diagnostic Message is valid
 * @param sourceAddress		currently registered source address on the socket
 * @param data				message which was received
 */
unsigned char parseDiagnosticMessage(unsigned char sourceAddress [2], unsigned char data[64]) {
	//Check if the received SA is registered on the socket
	if(data[8] != sourceAddress[0] || data[9] != sourceAddress[1]) {
		//SA of received message is not registered on this TCP_DATA socket
		return 0x02;
	}
	
	//Check if the TA is known
	//TODO: remove hard coded value
	if(data[10] != 0xE0 || data[11] != 0x00) {
		//TA is not known
		return 0x03;
	}
	
	//Check if diagnostic Message size is generally supported by the target networt/TP	
	//Check if the currently available target buffer can handle the diagnostic message	
	//Check if TA is reachable/Routing to TA is already configured validy
	
	//Pass the diagnostic message to the target network/transport layer
	
	//return positive ack code
	return 0x00;
}

/**
 * Creates a diagnostic message positive/negative acknowledgment message
 * @param type				defines positive/negative payload type
 * @param sourceAddress		logical address of the receiver of the previous diagnostic message
 * @param targetAddress		logical address of the sender of the previous diagnostic message
 * @param responseCode		positive or negative acknowledge code
 */
unsigned char* createDiagnosticACK(PayloadType type, unsigned char sourceAddress [2],
								   unsigned char targetAddress [2], unsigned char responseCode) {
	
	unsigned char* message = createGenericHeader(type, _DiagnosticPositiveACKLength);
	
	//add source address to the message
	message[8] = sourceAddress[0];
	message[9] = sourceAddress[1];
	
	//add target address to the message
	message[10] = targetAddress[0];
	message[11] = targetAddress[1];
	
	//add positive or negative acknowledge code to the message
	message[12] = responseCode;
	
	return message;
}