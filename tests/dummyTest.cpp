/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dummyTest.cpp
 * Author: linuxvm
 *
 * Created on 27. Juli 2018, 17:15
 */

#include <stdlib.h>
#include <iostream>
#include <gtest/gtest.h>
using namespace std;

/*
 * Simple C++ Test Suite
 */

TEST(DummyTest, dummy)
{
    int i = 6;
    
    EXPECT_EQ(5, i);
}


int main(int argc, char** argv) {
    
    cout << "hello" << endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    
}

