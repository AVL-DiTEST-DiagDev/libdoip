#!/bin/sh
docker run --rm --privileged multiarch/qemu-user-static:register --reset
docker build -t libdoipbuild:rpi -f Dockerfile.rpi .
docker run --rm -v "$(pwd):/proj" -w /proj libdoipbuild:rpi make $@