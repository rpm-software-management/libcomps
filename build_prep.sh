#!/usr/bin/sh
if [ "$#" -eq "0" ]; then
    GITREV=`git rev-parse --short HEAD`
else
    GITREV=$1
fi
echo "Generate tarball for revision: $GITREV"
git archive ${GITREV} --format=tar.gz --prefix=libcomps/ >libcomps-${GITREV}.tar.xz

$(
export GITREV=$GITREV
envsubst <"libcomps.spec.in" >"libcomps.spec"
)

#git archive --format=tar.gz  --prefix libcomps/ HEAD:libcomps > libcomps.tar.gz
