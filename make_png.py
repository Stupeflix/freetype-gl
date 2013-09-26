#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import imp
import Image


path = sys.argv[1]

with open(path + '.py', 'rb') as fp:
  meta = imp.load_module(
    'meta', fp, path,
    ('.py', 'rb', imp.PY_SOURCE)
  )

w = meta.data['atlas_width']
h = meta.data['atlas_height']

print "  Generate \"" + path + ".png\" (" + str(w) + "x" + str(h) + ").\n"

data = open(path + '.atlas').read()
i = Image.fromstring("L", (w,h), data)
i.convert('RGBA')
i.save(path + '.png')
