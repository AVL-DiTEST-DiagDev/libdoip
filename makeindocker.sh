#!/bin/sh
docker build -t libdoipbuild .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild make $@

