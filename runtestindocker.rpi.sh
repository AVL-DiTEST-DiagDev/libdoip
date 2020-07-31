#!/bin/sh
docker build -t libdoipbuild:rpi -f Dockerfile.rpi .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild:rpi ./runTest --gtest_output="xml:./testOutput.xml"

