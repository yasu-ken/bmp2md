
# BMP to MegaDrive Graphic Converter


What is bmp2md?
------------------
bmp2md converts BMP File to a format that can be used on the MegaDrive.
it is used for mega drive game development.
this is command line tool.

Using the upper left 128x128 dot of the bmp image, output in the method used by Mega-dora.


Command Line
-------------

USE: bmp2md -T? <????.bmp> <????.text (or) ????.bin >

bmp2md src.BMP -T0 src.BMP output.TXT
bmp2md src.BMP -T1 output.BIN
bmp2md src.BMP -T3 output.BIN


Hints
-----
Read the binary file from the assembler with ".incbin"

If you use only a small amount of images, extract the required amount from the text output file.
