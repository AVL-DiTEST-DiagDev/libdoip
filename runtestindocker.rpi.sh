#!/bin/sh
docker build -t libdoipbuild:rpi -f Dockerfile.rpi .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild:rpi ./build_test/runTest --gtest_output="xml:./testOutput.xml"

