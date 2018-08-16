
#include "DoIPClient_h.h"

/*
 *Set up the connection between client and server
 */

void DoIPClient::startTcpConnection() {

    const char* ipAddr="127.0.0.1";
    bool connectedFlag=false;
    _sockFd= socket(AF_INET,SOCK_STREAM,0);   
    
    if(_sockFd>=0)
    {
        cout<< "Client-Socket wurde angelegt." <<endl;
    
        _serverAddr.sin_family=AF_INET;
        _serverAddr.sin_port=htons(_serverPortNr);
        inet_aton(ipAddr,&(_serverAddr.sin_addr)); 
        
        while(!connectedFlag)
        {
            _connected= connect(_sockFd,(struct sockaddr *) &_serverAddr,sizeof(_serverAddr));
            if(_connected!=-1)
            {
                connectedFlag=true;
                cout<<"Die Verbindung zum Server wurde hergestellt"<<endl;
            }
        }  
    }   
}

void DoIPClient::startUdpConnection(){
    
    const char* ipAddr="127.0.0.1";
    _sockFd= socket(AF_INET,SOCK_DGRAM, 0); 
    
    if(_sockFd>= 0)
    {
        cout <<"Client-UDP-Socket wurde angelegt." <<endl;
        
        _serverAddr.sin_family=AF_INET;
        _serverAddr.sin_port=htons(_serverPortNr);
        inet_aton(ipAddr,&(_serverAddr.sin_addr)); 
    }
}

/*
 * closes the client-socket
 */
void DoIPClient::closeTcpConnection(){
    
    close(_sockFd);
  
}

void DoIPClient::closeUdpConnection(){
    
    close(_sockFd);
}

/*
 *Build the Routing-Activation-Request for server
 */
const pair<int,unsigned char*>* DoIPClient::buildRoutingActivationRequest() {
    
   pair <int,unsigned char*>* rareqWithLength= new pair<int,unsigned char*>();
   int rareqLength=15;
   unsigned char * rareq= new unsigned char[rareqLength];
  
   //Generic Header
   rareq[0]=0x02;  //Protocol Version
   rareq[1]=0xFD;  //Inverse Protocol Version
   rareq[2]=0x00;  //Payload-Type
   rareq[3]=0x05;
   rareq[4]=0x00;  //Payload-Length
   rareq[5]=0x00;  
   rareq[6]=0x00;
   rareq[7]=0x07;
   
   //Payload-Type specific message-content
   rareq[8]=0x0E;  //Source Address
   rareq[9]=0x00;
   rareq[10]=0x00; //Activation-Type
   rareq[11]=0x00; //Reserved ISO(default)
   rareq[12]=0x00;
   rareq[13]=0x00;
   rareq[14]=0x00;
   
   rareqWithLength->first=rareqLength;
   rareqWithLength->second=rareq;
  
   return rareqWithLength;
}

/*
 * Send the builded request over the tcp-connection to server
 */
void DoIPClient::sendRoutingActivationRequest() {
    
    
    const pair <int,unsigned char*>* rareqWithLength=buildRoutingActivationRequest();
    int sendedBytes;
    sendedBytes=write(_sockFd,rareqWithLength->second,rareqWithLength->first);
  
   
    
}

/*
 * Receive a routing-activation-response from server
 */
void DoIPClient::receiveRoutingActivationResponse() {
    
    int readedBytes;
    readedBytes= read(_sockFd,_receivedData,_maxDataSize);
  
   for(int i=0;i<readedBytes;i++)
    {
       cout<<(int)_receivedData[i]<<endl;
    }    
}

void DoIPClient::receiveUdpMessage() {
    
    unsigned int length = sizeof(_serverAddr);
    
    int readedBytes;
    readedBytes = recvfrom(_sockFd, _receivedData, _maxDataSize, 0, (struct sockaddr*)&_serverAddr, &length);
    
    for(int i=0;i<readedBytes;i++)
    {
       cout<<(int)_receivedData[i]<<endl;
    } 
    
    if(_receivedData[2] == 0x00 && _receivedData[3] == 0x04)
    {
        parseVIResponseInformation(_receivedData);
    }
    
}

const pair<int,unsigned char*>* DoIPClient::buildVehicleIdentificationRequest(){
    
   pair <int,unsigned char*>* rareqWithLength= new pair<int,unsigned char*>();
   int rareqLength= 8;
   unsigned char * rareq= new unsigned char[rareqLength];
  
    //Generic Header
   rareq[0]=0x02;  //Protocol Version
   rareq[1]=0xFD;  //Inverse Protocol Version
   rareq[2]=0x00;  //Payload-Type
   rareq[3]=0x01;
   rareq[4]=0x00;  //Payload-Length
   rareq[5]=0x00;  
   rareq[6]=0x00;
   rareq[7]=0x00;
    
   rareqWithLength->first=rareqLength;
   rareqWithLength->second=rareq;
  
   return rareqWithLength;
    
}

void DoIPClient::sendVehicleIdentificationRequest(const char* address){
    
     const char* ipAddr= address;
     
     inet_aton(ipAddr,&(_serverAddr.sin_addr));
    
    const pair <int,unsigned char*>* rareqWithLength=buildVehicleIdentificationRequest();
    int sendedBytes;
    sendedBytes=sendto(_sockFd, rareqWithLength->second,rareqWithLength->first, 0, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr));
    
}

/*
* Getter for _sockFD
*/
int DoIPClient::getSockFd() {
	return _sockFd;
}

/*
* Getter for _connected
*/
int DoIPClient::getConnected() {
	
}

void DoIPClient::parseVIResponseInformation(unsigned char* data){
    
    //VIN
    int j = 0;
    for(int i = 8; i <= 24; i++)
    {      
        VINResult[j] = data[i];
        j++;
    }
    
    //Logical Adress
    j = 0;
    for(int i = 25; i <= 26; i++)
    {
        LogicalAddressResult[j] = data[i];
        j++;
    }
      
    //EID
    j = 0;
    for(int i = 27; i <= 32; i++)
    {
        EIDResult[j] = data[i];
        j++;
    }
    
    //GID
    j = 0;
    for(int i = 33; i <= 38; i++)
    {
        GIDResult[j] = data[i];
        j++;
    }
    
    //FurtherActionRequest
    FurtherActionReqResult = data[39];
    
}