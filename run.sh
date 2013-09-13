#!/bin/sh

NAME=$1
FONT_NAME=$2
RESOLUTION=$3

sleep 1
mkdir -p build && \
cd build && \
cmake .. && \
make && \
./compute-distmap ${FONT_NAME} ${NAME} ${RESOLUTION} && \
python make_png.py ${NAME}