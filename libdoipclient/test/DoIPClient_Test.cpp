#include "DoIPClient_h.h"
#include <gtest/gtest.h>

class DoIPClient_Test : public ::testing::Test{
   
    protected:
        void SetUp() override {
        }
    
    DoIPClient client1;   
};

/*
 * Check if the client connected with the server
 */
TEST_F(DoIPClient_Test,VerifyTcpConnectionState){
        
    ASSERT_GE(client1.getSockFd(),0);
    EXPECT_GE(client1.getConnected(),0);       
} 

