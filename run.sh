#!/bin/sh

NAME=$1
FONT_NAME=$2
mkdir -p build && \
cd build && \
cmake .. && \
make && \
./compute-distmap ${FONT_NAME} ${NAME} && \
python make_png.py ${NAME} && \
eog ${NAME}.png