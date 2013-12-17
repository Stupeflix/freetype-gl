#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import Image
import simplejson as json
# import codecs

path = sys.argv[1]

with open(path + ".json", 'rb') as f:
  meta = json.load(f)

w = meta['atlas_width']
h = meta['atlas_height']

print "  Generate \"" + path + ".png\" (" + str(w) + "x" + str(h) + ").\n"

data = open(path + '.atlas').read()
i = Image.fromstring("L", (w,h), data)
i.convert('RGBA')
i.save(path + '.png')
