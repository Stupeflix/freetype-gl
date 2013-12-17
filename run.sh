#!/bin/sh

FONT_FILE=$1
RESOLUTION=$2

sleep 1
mkdir -p build && \
cd build && \
cmake .. && \
make && \
./compute-distmap ${FONT_FILE} ${RESOLUTION} && \
python make_png.py ${FONT_FILE} && \
cp ${FONT_FILE}.json ../fonts && \
cp ${FONT_FILE}.png ../fonts