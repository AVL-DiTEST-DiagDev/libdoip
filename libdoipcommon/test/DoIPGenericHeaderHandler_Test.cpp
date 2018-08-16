#include <gtest/gtest.h>
#include "DoIPGenericHeaderHandler.h"

class GenericHeaderTest : public ::testing::Test {
	public:
		unsigned char* request;

	protected:
		void SetUp() override {
			request = new unsigned char[15];
			request[0] = 0x01;
			request[1] = 0xFE;
			request[2] = 0x00;
			request[3] = 0x05;
			request[4] = 0x00;
			request[5] = 0x00;
			request[6] = 0x00;
			request[7] = 0x07;
			request[8] = 0x0F;
			request[9] = 0x12;
			request[10] = 0x00;
			request[11] = 0x00;
			request[12] = 0x00;
			request[13] = 0x00;
			request[14] = 0x00;
		}
};

/*
* Checks if a wrong synchronization pattern leads to the correct response type and NACK code (0x00)
*/
TEST_F(GenericHeaderTest, WrongSynchronizationPattern) {
	//Set wrong inverse protocol version
	request[1] = 0x33;
	GenericHeaderAction action = parseGenericHeader(request, 15);
	ASSERT_EQ(action.type, PayloadType::NEGATIVEACK) << "returned payload type is wrong";
	ASSERT_EQ(action.value, 0x00) << "returned NACK code is wrong";

	//Set currently not supported protocol version
	request[1] = 0xFE;
	request[0] = 0x04;
	action = parseGenericHeader(request, 15);
	ASSERT_EQ(action.type, PayloadType::NEGATIVEACK) << "returned payload type is wrong";
	ASSERT_EQ(action.value, 0x00) << "returned NACK code is wrong";
}

/*
* Checks if a unknown payload type leads to the correct response type and NACK code (0x01)
*/
TEST_F(GenericHeaderTest, UnknownPayloadType) {
	//Set unknown payload type (0x0010)
	request[3] = 0x10;
	GenericHeaderAction action = parseGenericHeader(request, 15);
	ASSERT_EQ(action.type, PayloadType::NEGATIVEACK);
	ASSERT_EQ(action.value, 0x01);
}

/*
* Checks if a too long message returns the correct response type and NACK code (0x02)
*/
TEST_F(GenericHeaderTest, MessageLength) {
	GenericHeaderAction action = parseGenericHeader(request, 4096);
	ASSERT_EQ(action.type, PayloadType::NEGATIVEACK);
	ASSERT_EQ(action.value, 0x02);
}

/*
* Checks if a wrong routing activation payload length return the correct response type and NACK code (0x04)
*/
TEST_F(GenericHeaderTest, WrongRoutingActivationLength) {
	//currently the payload type specific length is tested with subtracting the generic header length
	//from the received message length and compare it with a hardcoded value, 7 for routing activation request
	GenericHeaderAction action = parseGenericHeader(request, 20);
	ASSERT_EQ(action.type, PayloadType::NEGATIVEACK);
	ASSERT_EQ(action.value, 0x04);
}

/*
* Checks if a valid generic header leads to the correct response type
*/
TEST_F(GenericHeaderTest, ValidGenericHeader) {
	GenericHeaderAction action = parseGenericHeader(request, 15);
	ASSERT_NE(action.type, PayloadType::NEGATIVEACK);
}