#!/bin/bash

# Build script

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ! -d $DIR/build ]; then
	mkdir $DIR/build
fi

cd $DIR/build

cmake $DIR/src
make -j
