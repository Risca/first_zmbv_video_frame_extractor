#!/bin/sh
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <AVI file> <output file>"
    exit 1
fi

./extract < "$1" | zlib-flate -uncompress > "$2"
