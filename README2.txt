
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

?????.bmp
    Source filename. Windows BMP Format.
    palettype is nonarchive-16color or 256color.

-T GraphicType
	-T0  Sprite4x4(Vertical arrangement) 4x4(16)patterns (=128x128dot)
	-T1  Sprite2x2(Vertical arrangement) 8x8(64)patterns (=128x128dot)
	-T2  BG(Horizontal arrangement) 16x16patterns(=128x128dot)

?????.txt
	Output Text filename.

?????.bin
	Output Binary filename.


>bmp2md src.BMP -T0 src.BMP output.TXT

>bmp2md src.BMP -T1 output.BIN

>bmp2md src.BMP -T3 output.BIN



Hints
-----
Read the binary file from the assembler with ".incbin"

If you use only a small amount of images, extract the required amount from the text output file.



  BMP File(Windows)
  (0,0)-(127,127) --->MeGaDrive (8x8dot == 1chip)
                          Sprite 4x4chip x 16pattern
                          Sprite 2x2chip x 64pattern
                          BackGround 16x16chip(256chip)

+++++++++++++++++++++
 BMPFILE
+++++++++++++++++++++
  ++++++++++++++++++++++++
  +         +            +
  + 128x128 +            +
  +         +            +
  +         +            +
  +++++++++++            +
  +                      +
  +                      +
  +                      +
  +                      +
  ++++++++++++++++++++++++
  Upper left 128x128 dot without image is used.
  Only the first 16 colors are used.


+++++++++++++++++++++
 Sprite
+++++++++++++++++++++
   4x4TYPE (Vertical)

      4x4chip=1spr
      [0][4][8][c]
      [1][5][9][d]
      [2][6][a][e]
      [3][7][b][f]
       /|
      / |  16Pattern (4x4spr)
     [ 0 ][ 4 ][ 8 ][ C ]
     [ 1 ][ 5 ][ 9 ][ D ]
     [ 2 ][ 6 ][ A ][ E ]
     [ 3 ][ 7 ][ B ][ F ]
                   (128x128dot)

  2x2TYPE (Vertical)

      2x2chip=1spr
      [0][3]
      [1][4]
       /|
      / |  64Pattern (8x8spr)
     [00][08][10][18][20][28][30][38]
     [01][09][11][19][21][29][31][39]
     [02][0a][12][1a][22][2a][32][3a]
     [03][0b][13][1b][23][2b][33][3b]
     [04][0c][14][1c][24][2c][34][3c]
     [05][0d][15][1d][25][2d][35][3d]
     [06][0e][16][1e][26][2e][36][3e]
     [07][0f][17][1f][27][2f][37][3f]
                          (128x128dot)


+++++++++++++++++++++
 BG
+++++++++++++++++++++
  (1chip 8x8dot)
  [00][01][02][03][04][05][06][07][08][09][0a][0b][0c][0d][0e][0f]
  [10][11][12][13][14][15][16][17][18][19][1a][1b][1c][1d][1e][1f]
  [20][21][22][23][24][25][26][27][28][29][2a][2b][2c][2d][2e][2f]
  [30][31][32][33][34][35][36][37][38][39][3a][3b][3c][3d][3e][3f]
  [40][41][42][43][44][45][46][47][48][49][4a][4b][4c][4d][4e][4f]
  [50][51][52][53][54][55][56][57][58][59][5a][5b][5c][5d][5e][5f]
  [60][61][62][63][64][65][66][67][68][69][6a][6b][6c][6d][6e][6f]
  [70][71][72][73][74][75][76][77][78][79][7a][7b][7c][7d][7e][7f]
  [80][81][82][83][84][85][86][87][88][89][8a][8b][8c][8d][8e][8f]
  [90][91][92][93][94][95][96][97][98][99][9a][9b][9c][9d][9e][9f]
  [a0][a1][a2][a3][a4][a5][a6][a7][a8][a9][aa][ab][ac][ad][ae][af]
  [b0][b1][b2][b3][b4][b5][b6][b7][b8][b9][ba][bb][bc][bd][be][bf]
  [c0][c1][c2][c3][c4][c5][c6][c7][c8][c9][ca][cb][cc][cd][ce][cf]
  [d0][d1][d2][d3][d4][d5][d6][d7][d8][d9][da][db][dc][dd][de][df]
  [e0][e1][e2][e3][e4][e5][e6][e7][e8][e9][ea][eb][ec][ed][ee][ef]
  [f0][f1][f2][f3][f4][f5][f6][f7][f8][f9][fa][fb][fc][fd][fe][ff]
                                                   (16x16chip)==(128x128dot)

+++++++++++++++++++++
 MegaDrive(Genesis)
+++++++++++++++++++++

   1Charactor(4 Bytes x 8Line = 32 Bytes)
                  (x 8dot = 4 Bytes )
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
    ・・・・・・・・
  (y 8Line)

   2Dot
    LSB
    0 1 2 3 4 5 6 7 8 9 a b c d e f
   | Palet No.     | Palet No.     |



How to build bmp2md.c
----------------------
I use Borland C++ 5.5.

compile.bat
>echo off
>set path="C:\borland\bcc55\Bin"
>BCC32.EXE bmp2md.cpp > error.txt

I think it can be compiled with gcc.



Changes
-------
version 1.0   2022/2/2
    first release.

License
-------
This software is released under the MIT License, see LICENSE.txt.

(EOF)
