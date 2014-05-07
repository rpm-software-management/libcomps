#!/bin/sh

FILE=$(readlink -f $0)
DIRNAME=$(dirname $FILE)
echo "scons -Y $DIRNAME/scons/ $@"
scons -Y $DIRNAME/scons/ $@
