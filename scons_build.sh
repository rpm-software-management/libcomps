#!/bin/sh

FILE=$(readlink -f $0)
DIRNAME=$(dirname $FILE)
scons -Y $DIRNAME/scons/ $@
