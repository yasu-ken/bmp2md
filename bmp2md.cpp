
/*

BMP to MegaDrive Graphic Converter

ver.1 2022.2.2

  BMP File(Windows)
  (0,0)-(127,127) --->MeGaDrive (8x8dot == 1chip)
                          Sprite 4x4chip x 16pattern
                          BackGround 16x16chip(256chip)

*/


#include <conio.h>		// for getch();
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

//******************************************************************************
// BMP Hedder
//******************************************************************************

	//ヘッダ丸ごと読み込まず、項目一つずつ読みます(アラインメントのためずれてしまう)
struct BMAPHEADER_2 {
	unsigned char   bfType0;        //1 データ形式(B)
	unsigned char   bfType1;        //1 データ形式(M)
	unsigned long   bfSize;         //4 ファイルサイズ
	unsigned short  bfReserved1;    //2 予約
	unsigned short  bfReserved2;    //2 予約
	unsigned long   bfOffBits;      //4 ビットマップデータの開始オフセット
	unsigned long   biSize;         //4 ヘッダーのサイズ（以下のデータ）
	long            biWidth;        //4 水平ドット数
	long            biHeight;       //4 垂直ドット数
	unsigned short  biPlanes;       //2 プレーン数
	unsigned short  biBitCount;     //2 １ピクセル当たりのビット数(8)
	unsigned long   biCompression;  //4 圧縮形式（０＝無し）
	unsigned long   biSizeImage;    //4 ビットマップデータサイズ
	long            biXPixPerMeter; //4 水平解像度
	long            biYPixPerMeter; //4 垂直解像度
	unsigned long   biClrUsed;      //4 使用色数
	unsigned long   biClrImporant;  //4 重要な色？
	unsigned char   palet[256][4];  //  1024 256色パレット
}BitmapHedder;

int ReadSize;
int BmpWidth;
int BmpHeight;
unsigned char *BmpBuff;

//******************************************************************************
// func.
//******************************************************************************

//**************************************
// text save
//**************************************

void save1chara_txt( FILE *fp, unsigned long *ptr ,int cnt, int No)
{
	if( cnt==0 )
		fprintf( fp, "\tdc.l  $%08X  ; $%02X\n",ptr[0],No );
	else
		fprintf( fp, "\tdc.l  $%08X\n",ptr[0] );

	fprintf( fp, "\tdc.l  $%08X\n",ptr[1] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[2] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[3] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[4] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[5] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[6] );
	fprintf( fp, "\tdc.l  $%08X\n",ptr[7] );
	fprintf( fp, "\n",ptr[7] );
}

//**************************************
// read bmpfile
//**************************************

int bmp_read( FILE *fp )
{
	int i,j;
	unsigned char wok;

	fread(&BitmapHedder.bfType0        , 1,1,fp);
	fread(&BitmapHedder.bfType1        , 1,1,fp);
	fread(&BitmapHedder.bfSize         , 1,4,fp);
	fread(&BitmapHedder.bfReserved1    , 1,2,fp);
	fread(&BitmapHedder.bfReserved2    , 1,2,fp);
	fread(&BitmapHedder.bfOffBits      , 1,4,fp);
	fread(&BitmapHedder.biSize         , 1,4,fp);
	fread(&BitmapHedder.biWidth        , 1,4,fp);
	fread(&BitmapHedder.biHeight       , 1,4,fp);
	fread(&BitmapHedder.biPlanes       , 1,2,fp);
	fread(&BitmapHedder.biBitCount     , 1,2,fp);
	fread(&BitmapHedder.biCompression  , 1,4,fp);
	fread(&BitmapHedder.biSizeImage    , 1,4,fp);
	fread(&BitmapHedder.biXPixPerMeter , 1,4,fp);
	fread(&BitmapHedder.biYPixPerMeter , 1,4,fp);
	fread(&BitmapHedder.biClrUsed      , 1,4,fp);
	fread(&BitmapHedder.biClrImporant  , 1,4,fp);
	//fread(&BitmapHedder.palet          , 1,1024,fp);

	//色ビットが8(256色)でなく、4(16色)でもないとき
	if( BitmapHedder.biBitCount != 8 )
	{
		if( BitmapHedder.biBitCount != 4 )
		{
			printf ( "Color type must be 8-bit. ");
			return -1;
		}
	}

	//color bit 16 or 256
	//reading first 16 color.
	if( BitmapHedder.biBitCount == 8 )
		fread(&BitmapHedder.palet  , 1,1024,fp);  //1024=256x4
	else
		fread(&BitmapHedder.palet  , 1,64,fp);    //16x4

	BmpWidth  =  BitmapHedder.biWidth;
	BmpHeight =  BitmapHedder.biHeight;

	ReadSize  =  BmpWidth* BmpHeight;

	BmpBuff   =  new char[ReadSize];

	fread( BmpBuff,1,ReadSize,fp);


	//bmpが上下逆になってるため、さらに上下逆にします。
	//bmp is upside down, so turn it upside down.
	for( i= 0; i<(ReadSize/2); i+=BmpWidth ) //line
	{
		for( j=0; j<BmpWidth; j++ )  //yoko
		{

			wok = BmpBuff[i+j];
			BmpBuff[i+j] = BmpBuff[(ReadSize-BmpWidth-i)+j];
			BmpBuff[(ReadSize-BmpWidth-i)+j] = wok;
/*
			wok = BmpBuff[i+j];
			BmpBuff[i+j] = BmpBuff[ReadSize-i+j];
			BmpBuff[ReadSize-i+j] = wok;
*/
		}
	}

	return 0;
}


//******************************************************************************
// check filename
//******************************************************************************

int chk_fname( char *ptr){
	int i;
	for( i=0; i<1000; i++ ){
		if( ptr[i] == '\0' ) return -1;
		if( ptr[i] == '.' )
			if(( ptr[i+1] == 'B' )||( ptr[i+1] == 'b' ))
				if(( ptr[i+2] == 'M' )||( ptr[i+2] == 'm' ))
					if(( ptr[i+3] == 'P' )||( ptr[i+3] == 'p' ))
						return (1);  //bmp
		if( ptr[i] == '.' )
			if(( ptr[i+1] == 'T' )||( ptr[i+1] == 't' ))
				if(( ptr[i+2] == 'X' )||( ptr[i+2] == 'x' ))
					if(( ptr[i+3] == 'T' )||( ptr[i+3] == 't' ))
						return (2);  //txt

		if( ptr[i] == '.' )
			if(( ptr[i+1] == 'B' )||( ptr[i+1] == 'b' ))
				if(( ptr[i+2] == 'I' )||( ptr[i+2] == 'i' ))
					if(( ptr[i+3] == 'N' )||( ptr[i+3] == 'n' ))
						return (3);  //bin
		if( ptr[i] == '-')
			if(( ptr[i+1] == 'B' )||( ptr[i+1] == 'b' ))
				return (4);  //BG switch
	}
	return -1;
}

/**********************************************
	test-->hex
************************************************/

unsigned int texttohex( char *ptr )
{
	unsigned int val=0;

	for(;;){
		if( *ptr=='\0')break;

		val <<= 4;

		if(( *ptr>='0')&&( *ptr<='9' )){
            val += *ptr - '0';
		}
		if(( *ptr>='A')&&( *ptr<='F' )){
            val += 10 + (*ptr - 'A');
		}
		if(( *ptr>='a')&&( *ptr<='f' )){
            val += 10 + (*ptr - 'a');
		}

		ptr++;
	}
	return val;
}



//******************************************************************************
// const
//******************************************************************************

// bmp内 16個の4x4スプライトの位置
int spr4x4_in_bmp[16][2]={
	{  0,  0},{ 0, 32},{ 0, 64},{ 0, 96},
	{ 32,  0},{32, 32},{32, 64},{32, 96},
	{ 64,  0},{64, 32},{64, 64},{64, 96},
	{ 96,  0},{96, 32},{96, 64},{96, 96}
	};

//bmp内のソース位置
int chara4x4_in_spr[16][2]={  //line、xbyte
	{ 0, 0 },{ 0, 8},{ 0,16},{ 0,24},
	{ 8, 0 },{ 8, 8},{ 8,16},{ 8,24},
	{16, 0 },{16, 8},{16,16},{16,24},
	{24, 0 },{24, 8},{24,16},{24,24}};



// bmp内 64個の2x2スプライトの位置
int spr2x2_in_bmp[16*4][2]={
	{  0,  0},{  0, 16},{  0, 32},{  0, 48},{  0, 64},{  0, 80},{  0, 96},{  0,112},
	{ 16,  0},{ 16, 16},{ 16, 32},{ 16, 48},{ 16, 64},{ 16, 80},{ 16, 96},{ 16,112},
	{ 32,  0},{ 32, 16},{ 32, 32},{ 32, 48},{ 32, 64},{ 32, 80},{ 32, 96},{ 32,112},
	{ 48,  0},{ 48, 16},{ 48, 32},{ 48, 48},{ 48, 64},{ 48, 80},{ 48, 96},{ 48,112},
	{ 64,  0},{ 64, 16},{ 64, 32},{ 64, 48},{ 64, 64},{ 64, 80},{ 64, 96},{ 64,112},
	{ 80,  0},{ 80, 16},{ 80, 32},{ 80, 48},{ 80, 64},{ 80, 80},{ 80, 96},{ 80,112},
	{ 96,  0},{ 96, 16},{ 96, 32},{ 96, 48},{ 96, 64},{ 96, 80},{ 96, 96},{ 96,112},
	{112,  0},{112, 16},{112, 32},{112, 48},{112, 64},{112, 80},{112, 96},{112,112}};

int chara2x2_in_spr[4][2]={  //line、xbyte
	{ 0, 0 },{ 0, 8},
	{ 8, 0 },{ 8, 8}};






//******************************************************************************
// data
//******************************************************************************

//16x16ドットの作業領域
unsigned short dot_wok[256];


char *BmpFilename = NULL;
char *TxtFilename = NULL;
char *BinFilename = NULL;

unsigned long chara_wok[32];    //32byte== 1chara

#define TYPE_SPR44 0
#define TYPE_SPR22 1
#define TYPE_BG    2

char *mdtype_text[3]={
		"SPRITE4x4",
		"SPRITE2x2",
		"BG"
	};

int md_datatype= TYPE_SPR44;  //0 1 2

int outputfiletype = 0;   //0:bin 1:txt




//******************************************************************************
// main
//******************************************************************************

int main(int argc, char *argv[])
{
	FILE *fp;
	int i,rtn;

	if (argc < 2)
	{
		fprintf(stderr, "Usage:  %s xxxxx.bmp <-Tx> <xxxxx.txt> <xxxxx.bin> \n", argv[0]);
		fprintf(stderr, " -Tx :output datatype x=0:Sprite4x4(default) 1:2x2 2:BG \n");
		fprintf(stderr, " xxxxx.bmp  WindowsBitmap( nonArc/16 or 256 color)\n");
		fprintf(stderr, " xxxxx.txt  save to text format \n");
		fprintf(stderr, " xxxxx.bin  save to binary format(default)\n");
		fprintf(stderr, " >bmp2md source.bmp -T0 output.txt \n");

		exit(1);
	}

	//get Argment
	for( i=1 ; i<argc ; i++ )
	{
		if( argv[i][0]=='-')
		{
			if( argv[i][1]=='T')
			{
				md_datatype= texttohex( &argv[i][2] );  //0 1 2
				if((md_datatype > 2 )||(md_datatype<0)) md_datatype=0;
			}
		}
	}

	//get bmp filename
	for(i=1; i<argc ; i++ )
	{
		rtn = chk_fname( argv[i] );
		if ( rtn == 1 ){
			BmpFilename = argv[i];
		}
		else if( rtn == 2 ){
			TxtFilename = argv[i];
		}
		else if( rtn == 3 ){
			BinFilename = argv[i];
		}
		else{}
	}

	if( BmpFilename == NULL ){
		fprintf(stderr, "bmp file not found.\n");
		exit(1);
	}


	// bmpファイルを読み込みます
	if ((fp = fopen( BmpFilename , "rb")) == NULL) {
		printf("%s: can't open %s\n", argv[0], BmpFilename);
		exit(1);
	}
	rtn = bmp_read( fp );
	fclose(fp);
	if( rtn == -1 ){
		fprintf(stderr, "bmp read error.\n");
		exit(1);
	}


	if( TxtFilename != NULL ){
		outputfiletype = 1;   //0:bin 1:txt
		fp = fopen( TxtFilename , "wt");
	}
	else if( BinFilename != NULL ){
		outputfiletype = 0;   //0:bin 1:txt
		fp = fopen( BinFilename , "wb");
	}
	else{
		outputfiletype = 0;   //0:bin 1:txt
		fp = fopen( "output.bin" , "wb");
	}
	if (fp == NULL ){
		printf("%s: can't open outputfile%s\n", argv[0] );
		exit(1);
	}


	int j,k,lcnt,yoko;
	int ofsx,ofsy,xx,yy;
	unsigned long longwok,long2;


	if( outputfiletype == 1 )   //0:bin 1:txt
	{
		fprintf(fp,"\t; Bmp2MD (2022.01- )\n" );
		fprintf(fp,"\t; TYPE = %s \n", mdtype_text[md_datatype] );
	}

	if( md_datatype == TYPE_SPR44 ){
		//左上256x256の画像を 4x4の縦並びのスプライトとして出力します
		//bmpの中から、つまみとって、512バイトを作ります。
		for( j=0; j<16; j++ ){  //16個の4x4を出力
			ofsx= spr4x4_in_bmp[j][0];
			ofsy= spr4x4_in_bmp[j][1];

			for( i=0; i<16; i++ ){
				xx= chara4x4_in_spr[i][0];
				yy= chara4x4_in_spr[i][1];

				for( k=0; k<32; k++ ){
					chara_wok[k] = 0;    //32バイトぶん
				}

				for( lcnt=0; lcnt<8; lcnt++ ){
					longwok = 0;
					for( yoko=0; yoko<8; yoko+=2){
						longwok >>= 8;
						//1ドットを取り出します。0-15の数値のはず
						long2 = 0;
						long2 |= (( BmpBuff[(ofsy+yy+lcnt)*BmpWidth + (ofsx+xx+yoko)])<<4)&0x0f0;
						long2 |=  ( BmpBuff[(ofsy+yy+lcnt)*BmpWidth + (ofsx+xx+yoko+1)]) &0x0f;
						long2<<=24;

						longwok |= long2;
					}
					chara_wok[lcnt] = longwok;
				}
				//32バイト作成できました。ファイルに書き込みます
				if( outputfiletype == 1 )  {
					save1chara_txt( fp, chara_wok,i,j);
				}
				else{
					fwrite( chara_wok , sizeof(long),8,fp);  //4x8=32byte
				}
			}
		}
	}
	else if( md_datatype == TYPE_SPR22 ){
		for( j=0; j<64; j++ ){
			ofsx= spr2x2_in_bmp[j][0];
			ofsy= spr2x2_in_bmp[j][1];

			for( i=0; i<4; i++ ){
				xx= chara2x2_in_spr[i][0];
				yy= chara2x2_in_spr[i][1];

				for( k=0; k<32; k++ ){
					chara_wok[k] = 0;    //32バイトぶん
				}

				for( lcnt=0; lcnt<8; lcnt++ ){
					longwok = 0;
					for( yoko=0; yoko<8; yoko+=2){
						longwok >>= 8;
						//1ドットを取り出します。0-15の数値のはず
						long2 = 0;
						long2 |= (( BmpBuff[(ofsy+yy+lcnt)*BmpWidth + (ofsx+xx+yoko)])<<4)&0x0f0;
						long2 |=  ( BmpBuff[(ofsy+yy+lcnt)*BmpWidth + (ofsx+xx+yoko+1)]) &0x0f;
						long2<<=24;

						longwok |= long2;
					}
					chara_wok[lcnt] = longwok;
				}
				//32バイト作成できました。ファイルに書き込みます
				if( outputfiletype == 1 )  {
					save1chara_txt( fp, chara_wok,i,j);
				}
				else{
					fwrite( chara_wok , sizeof(long),8,fp);  //4x8=32byte
				}
			}
		}
	}
	else{
		//BG 横並びタイプ
		for( j=0; j<16; j++ ){  //16個の4x4を出力
			for( i=0; i<16; i++ ){
				xx= i*8;
				yy= j*8;

				for( k=0; k<32; k++ ){
					chara_wok[k] = 0;    //32バイトぶん
				}

				for( lcnt=0; lcnt<8; lcnt++ ){
					longwok = 0;
					for( yoko=0; yoko<8; yoko+=2){
						longwok >>= 8;
						//1ドットを取り出します。0-15の数値のはず
						long2 = 0;
						long2 |= (( BmpBuff[(yy+lcnt)*BmpWidth + (xx+yoko)])<<4)&0x0f0;
						long2 |=  ( BmpBuff[(yy+lcnt)*BmpWidth + (xx+yoko+1)]) &0x0f;
						long2<<=24;

						longwok |= long2;
					}
					chara_wok[lcnt] = longwok;
				}
				//32バイト作成できました。ファイルに書き込みます
				if( outputfiletype == 1 )  {
					save1chara_txt( fp, chara_wok,0xff,0xff);
				}
				else{
					fwrite( chara_wok , sizeof(long),8,fp);  //4x8=32byte
				}

			}
		}
	}
	fclose(fp);
}

