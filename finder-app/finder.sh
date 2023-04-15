#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Wrong number of arguments"
    echo "Needs two arguments: <directory> <searchstring>"
    exit 1
fi

FILESDIR=$1
SEARCHSTR=$2

if [ ! -d $FILESDIR ]; then
    echo "Error, $FILESDIR is not a path"
    exit 1
fi

NR_FILES=`find $FILESDIR -type f | wc -l`

NR_MATCHES=`cat $(find $FILESDIR -type f) | grep -c $SEARCHSTR`

echo The number of files are $NR_FILES and the number of matching lines are $NR_MATCHES

