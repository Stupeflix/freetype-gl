#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
# import json

import Image
meta = __import__(sys.argv[1])

# print sys.argv[1] + ".json"

# data = json.loads("{\"hello\": 42}")

# print data

w = meta.data['atlas_width']
h = meta.data['atlas_height']
data = open(sys.argv[1] + ".atlas").read()
i = Image.fromstring("L", (w,h), data)
i.convert('RGBA')
i.save(sys.argv[1] + ".png")
