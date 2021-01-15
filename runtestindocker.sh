#!/bin/sh
docker build -t libdoipbuild .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild ./build_test/runTest --gtest_output="xml:./testOutput.xml"

