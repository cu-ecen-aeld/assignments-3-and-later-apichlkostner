#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Wrong number of arguments"
    echo "Needs two arguments: <filepath> <writestring>"
    exit 1
fi

WRITEFILE=$1
WRITESTR=$2

WRITEDIR=`dirname $WRITEFILE`


if [ ! -d $WRITEDIR ]; then
    echo Creating directory $WRITEDIR for $WRITEFILE
    mkdir -p $WRITEDIR
fi

echo $WRITESTR > $WRITEFILE

if [ $? != 0 ]; then
    echo $WRITEFILE could not be created
    exit 1
fi
