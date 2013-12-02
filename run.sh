#!/bin/sh

FONT_FILE=$1
RESOLUTION=$2

sleep 1
mkdir -p build && \
cd build && \
cmake .. && \
make && \
./compute-distmap ${FONT_FILE} ${RESOLUTION} && \
cp ${FONT_FILE}.py ../fonts
