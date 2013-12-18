DistmapGen
==========

How to generate a distmap
-------------------------

The first thing to do is to add your font into the fonts/ folder:

$ cp myFont.ttf fonts/

To generate a distmap for your font, just use run.sh to build everting and generate the font:

./run.sh FONT_FILE INFILE [RESOLUTION]

- FONT_FILE - The font file to use for generation (with league.ttf, league.ttf.py and league.ttf.png will be created).
- RESOLUTUION - Atlas are generated with size which are powers of two, so you can ajust glyph resolutions to fill all the picture.

### Example:

$ ./run.sh fonts/league.ttf 80

$ open fonts/league.ttf.{png,py}