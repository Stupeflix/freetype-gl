#!/usr/bin/env python

import sys

import Image
meta = __import__(sys.argv[1])

w = meta.data['atlas_width']
h = meta.data['atlas_height']
i = Image.fromstring("L", (w,h), open(sys.argv[1] + ".atlas").read())
i.save(sys.argv[1] + ".png")