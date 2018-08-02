
#include "DoIPClient_h.h"

class DoIPClient_Test : public ::testing::Test{
   
    protected:
        void SetUp() override {
            client1.startTcpConnection();
        }
    
    DoIPClient client1;   
};

/*
 * Check if the client connected with the server
 */
TEST_F(DoIPClient_Test,VerifyTcpConnectionState){
        
    ASSERT_GE(client1._sockFd,0);
    EXPECT_GE(client1._connected,0);       
} 

