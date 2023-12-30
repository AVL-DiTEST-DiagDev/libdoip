#include <gtest/gtest.h>
#include "VehicleIdentificationHandler.h"
#include <stdint.h>
#include <string>
#include "DoIPServer.h"

using namespace std;

class VehicleIdentificationHandlerTest : public ::testing::Test {
	public:
		string matchingVIN = "MatchingVin_12345";
		string shortVIN = "shortVin";
		string shortVINPadded = "shortVin000000000";
		unsigned char EID[6] = {0, 0, 0, 0, 0, 0};
		unsigned char GID[6] = {0, 0, 0, 0, 0, 0};
		unsigned char far = 0;
	protected:
		void SetUp() override {
		}
};

/*
 * Checks if a VIN with 17 bytes matches correctly the input data
 */
TEST_F(VehicleIdentificationHandlerTest, Vin17Bytes) {
	// Call function under test to create message
	unsigned char * message = createVehicleIdentificationResponse(matchingVIN, 0, EID, GID, far);
	
	// Extract VIN from created test message
	char tempvin[18]; // Need 1 byte more for \0 at the end for parsing via string()
        for(int i=0; i<=16; i++) {
		tempvin[i] = message[i+8];
	}
	tempvin[17] = 0; // Mark end of string
	
	delete[] message;
	
	// Assert that extracted VIN matches input data
	string expected = string(tempvin);
	EXPECT_EQ(matchingVIN, expected) << "Setting VIN with 17 bytes failed";
}

/*
 * Checks if a VIN < 17 bytes is padded correctly with zero bytes
 */
TEST_F(VehicleIdentificationHandlerTest, VinLessThan17Bytes) {
	// Call function under test to create message
	unsigned char * message = createVehicleIdentificationResponse(shortVIN, 0, EID, GID, far);
	
	// Extract VIN from created test message
	char c_tempvin[18];
        for(int i=0; i<=16; i++) {
		c_tempvin[i] = message[i+8];
	}
	c_tempvin[17] = 0; // Mark end of string

	// Create expected string value
	string actualVin = string(c_tempvin);

	// Assert that extracted VIN matches input data
	EXPECT_EQ(shortVINPadded, actualVin) << "Setting VIN with < 17 bytes failed";
	
	// Assert message after shortVin bytes is padded with '0'
	for(int i=16; i<=24; i++) {
		EXPECT_EQ(message[i], '0') << "VIN not correctly padded at byte: " << i;
	}
	delete[] message;
}

