/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2/2016
 *
 * 	@file 			gif2bmp.h
 *
 *	@brief			Project KKO - GIF to BMP conversion. 
 *	@details		Library written in C/C++. It is for conversion from GIF (GIF89a) to BMP image format.
 *					The input image in format GIF is encoded with LZW compression method.
 *					There is no compression used in the BMP file as a result.
 */


#ifndef GIF2BMP_H
#define GIF2BMP_H

/* 
 *	For printing a debug messages into stderr,
 *	use 'make debug' or add -DDEBUG to compiler flags.
 */
#if(DEBUG)
 	#define DEBUG_LOG(_x)	((void)_x)
#else
	#define DEBUG_LOG(_x)	((void)0)
#endif

#include <stdio.h>
#include <stdint.h>

#include <string.h>
#include <sstream>

#include <sys/types.h>
#include <ctype.h>

#include <vector>

using namespace std;

typedef uint8_t BYTE;	// 1 byte
typedef uint16_t WORD;	// 2 byte
typedef uint32_t DWORD;	// 4 byte


/* Constants in GIF file structure for indexes or lengths */
#define LOG_SCREEN_WIDTH_INDEX 6
#define LOG_SCREEN_HEIGHT_INDEX 8
#define LOG_SCREEN_PKTFIELDS_INDEX 10
#define LOG_SCREEN_BCKGCOL_INDEX 11
#define LOG_SCREEN_PIXEL_ASPECT_RATIO_INDEX 12

#define GCT_INDEX 13
#define CGE_SIZE 8

#define EXTENSION_INTRODUCER 0x21

#define COMM_EXT_APP_IDENTIF_SIZE 8
#define COMM_EXT_APP_AUTH_CODE_SIZE 3


/* BMP and GIF lengths */
typedef struct {
	int64_t bmpSize;
	int64_t long gifSize; // int64_t long gifSize;
} tGIF2BMP;

/* Logical screen - packet fields */
typedef struct {
	BYTE GCTFlag;
	BYTE colorResolution;
	BYTE sortFlag;
	WORD GCTSize; 	// The size is calculated as 2^(valueInVector+1) 
					// therefore WORD, max 256 triplets (RGB, every color is BYTE)
} tPackedFieldsLS;

/* Logical screen */
typedef struct {
	WORD width;
	WORD height;
	tPackedFieldsLS packetFields;
	BYTE backgroundColorIndex;
	BYTE pixelAspectRatio;
} tGIFLogicalScreen;

/* Local/Global Color Table - colors */
typedef struct {
	BYTE red;
	BYTE green;
	BYTE blue;
} tColorTable;

/* Image Descriptor - packet fields */
typedef struct {
	BYTE LCTFlag;
	BYTE interlaceFlag;
	BYTE sortFlag;
	BYTE reserved;
	WORD LCTSize;
} tPacketFieldsImgDescr;

/* Image Descriptor */
typedef struct {
	BYTE imgSeparator;
	WORD imgLeftPos;
	WORD imgTopPos;
	WORD imgWidth;
	WORD imgHeight;
	tPacketFieldsImgDescr packetFields;
	BYTE LZWminCodeSize;
	vector<BYTE> imageData; // Finally the image data encoded with LZW!
	BYTE blockTerminator;
} tImageDescriptor;

/* Global Control Extension - packet fields */
typedef struct {
	BYTE reserved;
	BYTE disposalMethod;
	BYTE userInputFlag;
	BYTE transparentColorFlag;
} tPacketFieldsGCE;

/* Graphic Control Extension */
typedef struct {
	BYTE extensionIntroducer;
	BYTE graphicControlLabel;
	BYTE blockSize;
	tPacketFieldsGCE packetFields;
	WORD delayTime;
	BYTE transparentColorIndex;
	BYTE blockTerminator;
} tGraphicControlExt;

/* Comment Extension */
typedef struct {
	BYTE extensionIntroducer;
	BYTE commentLabel;
	BYTE blockSize;
	vector<BYTE> data; // 1-255 bytes
	BYTE blockTerminator;
} tCommentExt;

/* Plain Text Extension */
typedef struct {
	BYTE extensionIntroducer;
	BYTE plainTextLabel;
	BYTE blockSize;
	WORD textGrifLeftPos;
	WORD textGrifTopPos;
	WORD textGridWidth;
	WORD textGridHeight;
	BYTE charCellWidth;
	BYTE charCellHeight;
	BYTE textForegroundColIndex;
	BYTE textBackgroundColIndex;
	vector<BYTE> plainTextData; // 1-255 bytes
	BYTE blockTerminator;
} tPlainTextExt;

/* Application Extension */
typedef struct {
	BYTE extensionIntroducer;
	BYTE applicationLabel;
	BYTE blockSize;
	BYTE appIdentif  [8];
	BYTE appAuthCode [3];
	vector<BYTE> appData;
	BYTE blockTerminator;
} tApplicationExt;

/* Extensions, they can occur multiple times, therefore all saved in vectors */
typedef struct {
	vector<tGraphicControlExt> graphicControl;
	vector<tCommentExt> comment;
	vector<tPlainTextExt> plainText;
	vector<tApplicationExt> application;
} tExtensions;


/* GIF header, main structure for GIF processing.
	GIF consist of:
		* Logical Screen
		* Global Color Table (GCT)	- optional, 0-1 time
		* Local Color Table (LCT)	- optional, 0-n times
		* Image Descriptor			- optional, 0-n times
		* Extensions				- optional, 0-n times:
			- Graphic Control
			- comment		(probably not needed in this project)
			- PlainText		(probably not needed in this project)
			- Application	(probably not needed in this project)
		* Trailer
*/
typedef struct {
	tGIFLogicalScreen logicScreen;
	vector<tColorTable> GCT;
	
	vector<tImageDescriptor> imgDescr;
	vector<tColorTable> LCT;

	tExtensions ext;
	BYTE trailer;
} tGIF;


/* BMP header - Microsoft Windows type of header */
typedef struct {
	DWORD width;
	DWORD height;
	WORD planes;
	WORD bitsPerPixel;
	DWORD compression;
	DWORD imgSize;
	DWORD XPixelsPerMeter;
	DWORD YPixelsPerMeter;
	DWORD numberOfColors;
	DWORD colorsImportant;
} tBMPHeader;


/* BMP consist of:
	* "BM" (magic constants)
	* Total size
	* Reserved1
	* Reserved2
	* Offset bits
	* Header size
	* Header (Old if size == 12. If size > 12 its Microsoft Windows BMP file, we will use in this project):
		- Width
		- Height
		- Planes
		- Bits per Pixel
		- Compression
		- Image size
		- X pixels per meter
		- Y pixels per meter
		- Number of Colors
		- Colors Important
	* Image data
*/
typedef struct {
	WORD signature;
	DWORD bmpSize;
	WORD reserved1;
	WORD reserved2;
	DWORD offset;
	DWORD headerSize;
	tBMPHeader header;
	vector<BYTE> imageData; 
} tBMP;


/* --------------- PROTOTYPES --------------- */
int gif2bmp(tGIF2BMP *gif2bmp, FILE *inputFile, FILE *outputFile);

/* --------------- GIF proessing --------------- */
void processLogicScreen(tGIF* gif, vector<int32_t>& vectorGifData);
void processGlobalColorTable(tGIF* gif, vector<int32_t>& vectorGifData);

void processGCExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index);
void processCommentExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index);
void processPlainTextExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index);
void processApplicationExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index);

void processImageDescriptor(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index);

uint64_t parseGIF(tGIF* gifData, vector<int32_t>& vectorGifData);

/* --------------- Functions for conversion --------------- */
string dec2binaryStringRev(uint16_t decVal);
string vector2binaryString(vector<uint8_t> imageData);
uint16_t binaryString2dec(string binData);

/* --------------- LZW decode algorithm --------------- */
void decodeGIFbyLZW(tGIF* gif, tBMP* bmp);

/* --------------- BMP processing --------------- */
void fillBMPdata(tGIF* gif, tBMP* bmp);
void interlaceOrder(uint16_t width, vector<uint8_t>& imageData);
vector<uint8_t> processBmpImgData(tGIF* gif, tBMP* bmp);
void createBMPfile(tBMP* bmp, FILE *outputFile);

#endif	/* GIF2BMP_H */
