/**
 *	@author			Ladislav Sulak (xsulak04@stud.fit.vutbr.cz)
 *	
 *	login 			xsulak04			
 *
 *	@date			2/2016
 *
 * 	@file 			gif2bmp.c
 *
 *	@brief			Project KKO - GIF to BMP conversion. 
 *	@details		Library written in C/C++. It is for conversion from GIF (GIF89a) to BMP image format.
 *					The input image in format GIF is encoded with LZW compression method.
 *					There is no compression used in the BMP file as a result.
 */


#include "gif2bmp.h"
#include <algorithm> 	// For vector/string reverse
#include <math.h>		// Pow


/**
 *	@synopsis	The main function which serves for conversion of GIT fo BMP file format.
 *
 * 	@param	gif2bmp stucture containing information about size of GIF and BMP files.
 *	@param 	inputFile input file (GIF).
 *	@param	outputFile output file (BMP).
 *	
 *	@return	'0' If the conversation was successful, '-1' otherwise (error, format not supported).
 */
int gif2bmp(tGIF2BMP *gif2bmp, FILE *inputFile, FILE *outputFile)
{
	// Fill decimal values from the input GIF file to the vector
	tGIF gifData;
	tBMP bmpData;

	int32_t ch;
	vector<int32_t> vectorGifData; 

	while((ch = fgetc(inputFile)) != EOF)
		vectorGifData.push_back(ch);

	if((gif2bmp->gifSize = parseGIF(&gifData, vectorGifData)) == 0) // 0 Bytes processed - error
		return -1;

	// Decode image data in GIF by method LZW.
	decodeGIFbyLZW(&gifData, &bmpData);

	// Fill structure for BMP file, data taken from GIF data, saved in structure.
	fillBMPdata(&gifData, &bmpData);

	// Create BMP file from the structure bmpData.
	createBMPfile(&bmpData, outputFile);

	// 54 is a size of values in BMP header (BMP without image data).
	gif2bmp->bmpSize = 54 + bmpData.imageData.size();
	
	return 0;
}


/**
 *	@synopsis	Logical Screen processing.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	
 *	@return	None.
 */
void processLogicScreen(tGIF* gif, vector<int32_t>& vectorGifData)
{
	gif->logicScreen.width = (vectorGifData[LOG_SCREEN_WIDTH_INDEX + 1] << 8) + vectorGifData[LOG_SCREEN_WIDTH_INDEX];
	gif->logicScreen.height = (vectorGifData[LOG_SCREEN_HEIGHT_INDEX + 1] << 8) + vectorGifData[LOG_SCREEN_HEIGHT_INDEX];

	// If its presented, GCT is located after logicScreen
	gif->logicScreen.packetFields.GCTFlag = (vectorGifData[LOG_SCREEN_PKTFIELDS_INDEX] & 128) >> 7; // bit 0

	// Original image had +1, see specification
	gif->logicScreen.packetFields.colorResolution = ((vectorGifData[LOG_SCREEN_PKTFIELDS_INDEX] & 112) >> 4) + 1; // bit 1-3
	
	gif->logicScreen.packetFields.sortFlag = (vectorGifData[LOG_SCREEN_PKTFIELDS_INDEX] & 8) >> 3; // bit 4

	// Size of GTS. The size itself is calculated from value in the vector: 2^(value+1) according to specification
	// Logical shift to left is just another writing for square function
	gif->logicScreen.packetFields.GCTSize = 1 << ((vectorGifData[LOG_SCREEN_PKTFIELDS_INDEX] & 7) + 1); // bit 5-7

	// See specification, aspectRatio = (value + 15) / 64
	gif->logicScreen.backgroundColorIndex = vectorGifData[LOG_SCREEN_BCKGCOL_INDEX];
	gif->logicScreen.pixelAspectRatio = (vectorGifData[LOG_SCREEN_BCKGCOL_INDEX] + 15) / 64;
}


/**
 *	@synopsis	Global Color Table processing. Function fills RGB data (3x BYTE) into GCT vector.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	
 *	@return	None.
 */
void processGlobalColorTable(tGIF* gif, vector<int32_t>& vectorGifData)
{
	DEBUG_LOG(fprintf(stderr, " --- Global Color Table --- \n"));
	for(uint16_t i = 0; i < gif->logicScreen.packetFields.GCTSize * 3; i += 3) {
		tColorTable GCTColors;
		GCTColors.red = vectorGifData[GCT_INDEX + i];
		GCTColors.green = vectorGifData[GCT_INDEX + i + 1];
		GCTColors.blue = vectorGifData[GCT_INDEX + i + 2];
		gif->GCT.push_back(GCTColors);

		DEBUG_LOG(fprintf(stderr, "%d %d %d \n", GCTColors.red, GCTColors.green, GCTColors.blue));
	}
}


/**
 *	@synopsis	Graphic Control Extension processing, can be multiple times.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	@param	index current position of the vectorGifData during processing.
 *
 *	@return	None.
 */
void processGCExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index)
{
	tGraphicControlExt GCExt;
	GCExt.extensionIntroducer = EXTENSION_INTRODUCER;
	GCExt.graphicControlLabel = 0xF9; // Specification, fixed value. The next, BlockSize is the same.
	GCExt.blockSize = 0x04;

	GCExt.packetFields.reserved = (vectorGifData[(*index) + 3] & 224) >> 5; 		// bit 0-2
	GCExt.packetFields.disposalMethod = (vectorGifData[(*index) + 3] & 28) >> 2; 	// bit 3-5
	GCExt.packetFields.userInputFlag = (vectorGifData[(*index) + 3] & 2) >> 1;		// bit 6
	GCExt.packetFields.transparentColorFlag = vectorGifData[(*index) + 3] & 1;		// bit 7

	GCExt.delayTime = (vectorGifData[(*index) + 5] << 8) + vectorGifData[(*index) + 4];
	GCExt.transparentColorIndex = vectorGifData[(*index) + 6];
	GCExt.blockTerminator = vectorGifData[(*index) + 7];

	gif->ext.graphicControl.push_back(GCExt);
	*index += CGE_SIZE;
}


/**
 *	@synopsis	Comment Extension (optional) processing, can be multiple times. Probably won't be used at all.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	@param	index current position of the vectorGifData during processing.
 *
 *	@return	None.
 */
void processCommentExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index)
{
	tCommentExt tmpCommentExt;
	tmpCommentExt.extensionIntroducer = EXTENSION_INTRODUCER;
	tmpCommentExt.commentLabel = 0xFE; // Fixed value, see specification
	tmpCommentExt.blockSize = vectorGifData[(*index) + 2];

	for(uint8_t i; i < tmpCommentExt.blockSize; i++) {
		tmpCommentExt.data.push_back(vectorGifData[(*index) + 4]);
		(*index)++;
	}
	tmpCommentExt.blockTerminator = vectorGifData[(*index)];
	(*index)++;

	gif->ext.comment.push_back(tmpCommentExt);
}


/**
 *	@synopsis	Plain Text Extension (optional) processing, can be multiple times. Probably won't be used at all.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	@param	index current position of the vectorGifData during processing.
 *
 *	@return	None.
 */
void processPlainTextExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index)
{
	tPlainTextExt tmpPlainTextExt;

	tmpPlainTextExt.extensionIntroducer = EXTENSION_INTRODUCER;
	tmpPlainTextExt.plainTextLabel = 0x01; // Fixed values, see specification
	tmpPlainTextExt.blockSize = 0x0C;

	tmpPlainTextExt.textGrifLeftPos = vectorGifData[(*index) + 3] + (vectorGifData[(*index) + 4] << 8);
	tmpPlainTextExt.textGrifTopPos = vectorGifData[(*index) + 5] + (vectorGifData[(*index) + 6] << 8);

	tmpPlainTextExt.textGridWidth = vectorGifData[(*index) + 7] + (vectorGifData[(*index) + 8] << 8);
	tmpPlainTextExt.textGridHeight = vectorGifData[(*index) + 9] + (vectorGifData[(*index) + 10] << 8);

	tmpPlainTextExt.charCellWidth = vectorGifData[(*index) + 11];
	tmpPlainTextExt.charCellHeight = vectorGifData[(*index) + 12];

	tmpPlainTextExt.textForegroundColIndex = vectorGifData[(*index) + 13];
	tmpPlainTextExt.textBackgroundColIndex = vectorGifData[(*index) + 14];

	uint8_t sizePlainTextBlock = vectorGifData[(*index) + 15];
	(*index) += 16;

	for(uint8_t i = 0; i < sizePlainTextBlock; i++) {
		tmpPlainTextExt.plainTextData.push_back(vectorGifData[(*index)]);
		(*index)++;
	}

	tmpPlainTextExt.blockTerminator = vectorGifData[(*index)];
	(*index)++;

	gif->ext.plainText.push_back(tmpPlainTextExt);
}


/**
 *	@synopsis	Application Extension (optional) processing, can be multiple times. Probably won't be used at all.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	@param	index current position of the vectorGifData during processing.
 *
 *	@return	None.
 */
void processApplicationExt(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index)
{
	tApplicationExt tmpAppExt;
	tmpAppExt.extensionIntroducer = EXTENSION_INTRODUCER;
	tmpAppExt.applicationLabel = 0xFF; // Fixed values, see specification
	tmpAppExt.blockSize = 0x0B;

	for(uint8_t i = 0; i < COMM_EXT_APP_IDENTIF_SIZE; i++) { 	// Application Identifire
		tmpAppExt.appIdentif[i] = vectorGifData[(*index) + 3];
		(*index)++;
	}

	for(uint8_t i = 0; i < COMM_EXT_APP_AUTH_CODE_SIZE; i++) { 	// Application Authentication Code
		tmpAppExt.appAuthCode[i] = vectorGifData[(*index)];
		(*index)++;
	}

	uint8_t sizeAppBlock = vectorGifData[(*index)];
	(*index)++;

	for(uint8_t i = 0; i < sizeAppBlock; i++) { 				// Application Data
		tmpAppExt.appData.push_back(vectorGifData[(*index)]);
		(*index)++;
	}

	tmpAppExt.blockTerminator = vectorGifData[(*index)];
	(*index)++;
	
	gif->ext.application.push_back(tmpAppExt);
}


/**
 *	@synopsis	Image Descriptor (optional) processing, can be multiple times according to specificaton.
 *				In this project we process only static (not animated) GIF files, so there is always just 1 img descriptor.
 *				If there is more than one, process them, save it to the vectors, 
 *				but use just the first one during producing the final BMP file.
 *				This function also processes Local Color Table (if there is any).
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *	@param	index current position of the vectorGifData during processing.
 *
 *	@return	None.
 */
void processImageDescriptor(tGIF* gif, vector<int32_t>& vectorGifData, uint64_t* index)
{
	uint8_t sizeDataBlock;

	tImageDescriptor tmpImgDescr;
	tmpImgDescr.imgSeparator = vectorGifData[(*index)];
	tmpImgDescr.imgLeftPos = vectorGifData[(*index) + 1] + (vectorGifData[(*index) + 2] << 8);
	tmpImgDescr.imgTopPos = vectorGifData[(*index) + 3] + (vectorGifData[(*index) + 4] << 8);
	tmpImgDescr.imgWidth = vectorGifData[(*index) + 5] + (vectorGifData[(*index) + 6] << 8);
	tmpImgDescr.imgHeight = vectorGifData[(*index) + 7] + (vectorGifData[(*index) + 8] << 8);

	tmpImgDescr.packetFields.LCTFlag = (vectorGifData[(*index) + 9] & 128) >> 7; 		// bit 0
	tmpImgDescr.packetFields.interlaceFlag = (vectorGifData[(*index) + 9] & 64) >> 6;	// bit 1
	tmpImgDescr.packetFields.sortFlag = (vectorGifData[(*index) + 9] & 32) >> 5;		// bit 2
	tmpImgDescr.packetFields.reserved = (vectorGifData[(*index) + 9] & 24) >> 3;		// bit 2-3
	tmpImgDescr.packetFields.LCTSize = 1 << ((vectorGifData[(*index) + 9] & 7) + 1);	// bit 4-7

	*index += 10; // There should begin next block.

	// There is also Local Color Table
	if(tmpImgDescr.packetFields.LCTFlag == 1) {
		DEBUG_LOG(fprintf(stderr, " --- Local Color Table --- \n"));
		for(uint16_t i = 0; i < tmpImgDescr.packetFields.LCTSize * 3; i += 3) {
			tColorTable LCTColors;
			LCTColors.red = vectorGifData[(*index) + i];
			LCTColors.green = vectorGifData[(*index) + i + 1];
			LCTColors.blue = vectorGifData[(*index) + i + 2];
			gif->LCT.push_back(LCTColors);

			DEBUG_LOG(fprintf(stderr, "%d %d %d \n", LCTColors.red, LCTColors.green, LCTColors.blue));
		}
		*index += tmpImgDescr.packetFields.LCTSize * 3;
	}

	tmpImgDescr.LZWminCodeSize = vectorGifData[(*index)];
	(*index)++;

	// Image Data, always end with 0x00, the first item is size of this image data block
	do {
		sizeDataBlock = vectorGifData[(*index)];
		(*index)++;
		for(uint8_t i = 0; i < sizeDataBlock; i++) {
			tmpImgDescr.imageData.push_back(vectorGifData[(*index)++]);
		}
	} while(vectorGifData[(*index)] != 0x00);

	tmpImgDescr.blockTerminator = vectorGifData[(*index)];
	gif->imgDescr.push_back(tmpImgDescr);
	(*index)++;
}


/**
 * @synopsys Function parses GIF file from vector containing GIF data. 
 *			 Everything is stored into structure of type tGIF.
 *	According to specification of GIF89a format: 
 *		- https://www.w3.org/Graphics/GIF/spec-gif89a.txt
 * 		but also 
 *		- http://www.onicos.com/staff/iz/formats/gif.html
 *		- https://en.wikipedia.org/wiki/GIF 
 *		- http://www.root.cz/clanky/anatomie-grafickeho-formatu-gif/
 *		- http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp
 *
 *	GIF consist of:
 *		"GIF 89a" (magic constants)
 *		Logical Screen
 *		Global Color Table (GCT)- optional, 0-1 times, at most 1 per each data stream, followed by Logical Screen
 *		Local Color Table (LCT)	- optional, 0-n times
 *		Image Descriptor		- optional, 0-n times, at most 1 per image in the Data Stream, unlimited number of images in each Data Stream
 *		Extensions				- optional, 0-n times:
 *			- Graphic Control
 *			- Comment
 *			- PlainText
 *			- Application
 *		Trailer
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	vectorGifData vector containing data from the input GIF file saved as a sequence of integer values.
 *
 *	@return	index number of processed bytes.
 */
uint64_t parseGIF(tGIF* gif, vector<int32_t>& vectorGifData)
{
	uint64_t index = 0; // Current hexadecimal value - used when there are variable blocks like extensions etc.

	if (vectorGifData[0] == 0x47 and // GIF in hexa
		vectorGifData[1] == 0x49 and
		vectorGifData[2] == 0x46)
	{
		if (vectorGifData[3] == 0x38 and // 89a or 87a in hexa
			(vectorGifData[4] == 0x39 or vectorGifData[4] == 0x37) and
			vectorGifData[5] == 0x61) {

			if (vectorGifData[4] == 0x37)
				fprintf(stderr, "The old version GIF87a doesn't need to be supported in this project!\n");

			processLogicScreen(gif, vectorGifData);

			// Global Color Table (if presented) - fill RGB hexa values into the vector of structures
			if(gif->logicScreen.packetFields.GCTFlag == 1) {

				processGlobalColorTable(gif, vectorGifData);
				index = GCT_INDEX + (gif->logicScreen.packetFields.GCTSize * 3); // After Logical Screen and GCT
			}
			else
				index = GCT_INDEX; // After Logical Screen but without GCT

			for (; index < vectorGifData.size();) {
				// Extensions
				if (vectorGifData[index] == 0x21) {
					if(vectorGifData[index + 1] == 0xF9 and vectorGifData[index + 2] == 0x04) {
						DEBUG_LOG(fprintf(stderr, "\n --- Graphic Control Extension in processing ---\n"));
						processGCExt(gif, vectorGifData, &index);
						continue;
					}
					else if(vectorGifData[index + 1] == 0xFE) {
						fprintf(stderr, "The input GIF contains Comment extension. \n");
						fprintf(stderr, "It doesn't need to be supported in this project, trying to process despite it.\n");
						
						DEBUG_LOG(fprintf(stderr, "\n --- Comment Extension in processing ---\n"));
						processCommentExt(gif, vectorGifData, &index);
						continue;
					}
					else if(vectorGifData[index + 1] == 0x01 and vectorGifData[index + 2] == 0x0C) {
						fprintf(stderr, "The input GIF contains Plain Text extension. \n");
						fprintf(stderr, "It doesn't need to be supported in this project, trying to process despite it.\n");
						
						DEBUG_LOG(fprintf(stderr, "\n --- Plain Text Extension in processing ---\n"));
						processPlainTextExt(gif, vectorGifData, &index);
						continue;
					}
					else if(vectorGifData[index + 1] == 0xFF and vectorGifData[index + 2] == 0x0B) {
						fprintf(stderr, "The input GIF contains Application extension. \n");
						fprintf(stderr, "It doesn't need to be supported in this project, trying to process despite it.\n");
						
						DEBUG_LOG(fprintf(stderr, "\n --- Application Extension in processing ---\n"));
						processApplicationExt(gif, vectorGifData, &index);
						continue;
					}
					else {
						fprintf(stderr, "The input GIF file is probably broken!\n");
						return 0;
					}
				}
				// Image Descriptor
				else if(vectorGifData[index] == 0x2C) {
					DEBUG_LOG(fprintf(stderr, "\n --- Image Descriptor in processing ---\n"));
					processImageDescriptor(gif, vectorGifData, &index);
					continue;
				}
				// Trailer (end of data)
				else if(vectorGifData[index] == 0x3B) {
					gif->trailer = 0x3B;
					index++;
					continue;
				}
				else {
					fprintf(stderr, "The input GIF file is probably broken!\n");
					return 0;
				}
			}
		}
	}

	else {
		fprintf(stderr, "The input file is not a valid GIF file!\n");
		return 0;
	}
	return index;
}


/**
 *	@synopsis	Function receives decimal number and converts it to binary sequence.
 *				The result is saved into string and reversed - because of LZW,
 *				we will process from the end of this string.
 *
 * 	@param	decVal decimal number which will be converted to binary string.
 *
 *	@return	converted string containing binary sequence.
 */
string dec2binaryStringRev(uint16_t decVal) {
	string convertedStr = "";
	uint16_t result = decVal;
	uint16_t value; // Tmp value used as a result of the modulo operation

	// Convert decimal value to binary and save it as a string.
	do {
		value = result % 2;
		result = result / 2;
		convertedStr += value + '0';
	} while(result != 0);

	// Fill with zeroes. We need to have as a result always 8 bits.
	for(uint16_t i = convertedStr.size(); i < 8; i++)
		convertedStr += '0';

	// The final result is in the reverse order.
	std::reverse(convertedStr.begin(), convertedStr.end());
	return convertedStr;
}


/**
 *	@synopsis	Function receives vector of GIF image data (WORD values) and  converts it to the string
 *				containing binary sequence which handles the function dec2binaryStringRev.
 *
 *	@param	imageData vector containing BYTE values of GIF image data. 
 *			
 *	@return	converted string containing the final binary sequence of image data.
 */
string vector2binaryString(vector<uint8_t> imageData) {
	string convertedStr = "";
	
	for(uint16_t i = 0; i < imageData.size(); i++)
		convertedStr += dec2binaryStringRev(imageData[i]);

	return convertedStr;
}


/**
 *	@synopsis	Function receives string of GIF image data (values encoded with LZW).
 *				This string is converted to a decimal number
 *
 *	@param	binData string containing binary values (GIF data encoded with LZW). 
 *			
 *	@return	decimal number which represends a string given from the parameter.
 */
uint16_t binaryString2dec(string binData) {
	uint16_t result = 0;

	for(uint8_t i = 0; i < binData.size(); i++)
		result += (binData[i] - '0') * pow(2, binData.size() - i - 1); // Bin values in string are in reverse order

	return result;
}


/**
 *	@synopsis	Function decodes GIF by LZW method. 
 *				All information is taken from GIF specification and also from here:
 *					http://www.matthewflickinger.com/lab/whatsinagif/lzw_image_data.asp
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param	bmp stucture for saving all information about the final BMP file.
 *			
 *	@return	decimal number which represends a string given from the parameter.
 */
void decodeGIFbyLZW(tGIF* gif, tBMP* bmp)
{
	vector<vector<uint16_t> > dictionary;
	vector<uint16_t> indexStream;	// Output values

	uint16_t dictionarySize;
	uint16_t clearCode, endOfInformCode;
	uint16_t code, prevCode;		// Save just indexes

	uint8_t currLZWCodeSize = gif->imgDescr[0].LZWminCodeSize + 1;
	uint16_t dictSizeMaxByLZW = pow(2.0, currLZWCodeSize) - 1; // Max dict size = (2^LZWminCodeSize) - 1

	// Our approach will be work in reverse order, because we will use LZW minimal code size, which can be 9+ bits.
	// That means, that we have to work sometimes with bits from another vector.
	// Instead of manipulating with vectors, we will store all hexadecimal data in reverse order as a binary string. 
	std::reverse(gif->imgDescr[0].imageData.begin(), gif->imgDescr[0].imageData.end());
	string codeStreamBinStr = vector2binaryString(gif->imgDescr[0].imageData);  

	// Initialize dictionary (code table)
	if(gif->logicScreen.packetFields.GCTFlag == 1) {
		dictionarySize = gif->logicScreen.packetFields.GCTSize + 2;
	}
	else if(gif->imgDescr[0].packetFields.LCTFlag == 1) {
		dictionarySize = gif->imgDescr[0].packetFields.LCTSize + 2;
	}

	for(uint16_t i = 0; i < dictionarySize; i++) {
		vector<uint16_t> item;
		dictionary.push_back(item);
	}

	for(uint16_t i = 0; i < dictionarySize; i++)
		dictionary[i].push_back(i);

	clearCode = dictionarySize - 2;
	endOfInformCode = dictionarySize - 1;

	// Get the first code
	code = binaryString2dec(codeStreamBinStr.substr(codeStreamBinStr.size() - currLZWCodeSize, codeStreamBinStr.size()));
	codeStreamBinStr.erase(codeStreamBinStr.size() - currLZWCodeSize,  codeStreamBinStr.size());

	// If it is Clear Code, read next value
	if(code == dictionary[clearCode][0]) {
		code = binaryString2dec(codeStreamBinStr.substr(codeStreamBinStr.size() - currLZWCodeSize, codeStreamBinStr.size()));
		codeStreamBinStr.erase(codeStreamBinStr.size() - currLZWCodeSize,  codeStreamBinStr.size());
	}
	prevCode = code;
	indexStream.push_back(dictionary[code][0]);

	while (codeStreamBinStr.size() != 0) {

		code = binaryString2dec(codeStreamBinStr.substr(codeStreamBinStr.size() - currLZWCodeSize, codeStreamBinStr.size()));
		codeStreamBinStr.erase(codeStreamBinStr.size() - currLZWCodeSize,  codeStreamBinStr.size());

		// Reinitialize dictionary to the original content
		if(code == dictionary[clearCode][0]) {
			currLZWCodeSize = gif->imgDescr[0].LZWminCodeSize + 1;
			dictSizeMaxByLZW = pow(2.0, currLZWCodeSize) - 1;

			// Initialize dictionary (code table)
			if(gif->logicScreen.packetFields.GCTFlag == 1) {
				dictionarySize = gif->logicScreen.packetFields.GCTSize + 2;
			}

			else if(gif->imgDescr[0].packetFields.LCTFlag == 1) {
				dictionarySize = gif->imgDescr[0].packetFields.LCTSize + 2;
			}

			dictionary.clear();
			for(uint16_t i = 0; i < dictionarySize; i++) {
				vector<uint16_t> item;
				dictionary.push_back(item);
			}

			for(uint16_t i = 0; i < dictionarySize; i++)
				dictionary[i].push_back(i);

			// Get the first code
			code = binaryString2dec(codeStreamBinStr.substr(codeStreamBinStr.size() - currLZWCodeSize, codeStreamBinStr.size()));
			codeStreamBinStr.erase(codeStreamBinStr.size() - currLZWCodeSize,  codeStreamBinStr.size());

			// If it is Clear Code, read next value
			if(code == dictionary[clearCode][0]) {
				code = binaryString2dec(codeStreamBinStr.substr(codeStreamBinStr.size() - currLZWCodeSize, codeStreamBinStr.size()));
				codeStreamBinStr.erase(codeStreamBinStr.size() - currLZWCodeSize,  codeStreamBinStr.size());
			}

			prevCode = code;
			indexStream.push_back(dictionary[code][0]);
		}

		// End of processing
		else if(code == endOfInformCode)
			break;

		else {

			// Item is already in the dictionary.
			if(code < dictionary.size()) {
				vector<uint16_t> newItem;
				dictionary.push_back(newItem);

				for(uint16_t i = 0; i < dictionary[code].size(); i++)
					indexStream.push_back(dictionary[code][i]);

				for(uint16_t i = 0; i < dictionary[prevCode].size(); i++)
					dictionary[dictionary.size()-1].push_back(dictionary[prevCode][i]);

				dictionary[dictionary.size()-1].push_back(dictionary[code][0]);
				prevCode = code;
			}

			// Item is not in the dictionary.
			else {
				vector<uint16_t> newItem;
				dictionary.push_back(newItem);

				for(uint16_t i = 0; i < dictionary[prevCode].size(); i++) {
					dictionary[dictionary.size()-1].push_back(dictionary[prevCode][i]);
					indexStream.push_back(dictionary[prevCode][i]);
				}

				dictionary[dictionary.size()-1].push_back(dictionary[prevCode][0]);
				prevCode = code;

				indexStream.push_back(dictionary[prevCode][0]);
			}

			if(dictionary.size()-1 == dictSizeMaxByLZW) {
				if(currLZWCodeSize < 12)
					currLZWCodeSize++;

				dictSizeMaxByLZW = pow(2.0, currLZWCodeSize) - 1;
			}
		}
	}

	DEBUG_LOG(fprintf(stderr, "\n --- Dictionary --- \n"));
	for(uint16_t i = 0; i < dictionary.size(); i++) {
		for(uint16_t j = 0; j < dictionary[i].size(); j++)
			DEBUG_LOG(fprintf(stderr, "%d ", dictionary[i][j]));
		DEBUG_LOG(fprintf(stderr, "\n"));
	}

	// Index stream to BMP image data.
	bmp->imageData.clear();

	DEBUG_LOG(fprintf(stderr, "\n --- Index stream decoded by LZW --- \n"));
	for(uint32_t i = 0; i < indexStream.size(); i++) {
		bmp->imageData.push_back(indexStream[i]);
		DEBUG_LOG(fprintf(stderr, "%d ", indexStream[i]));
	}
	DEBUG_LOG(fprintf(stderr, "\n"));
}


/**
 *	@synopsys Function creates BMP from structure containing GIF data.
 *	According to specification of BMP format:
 *		http://www.onicos.com/staff/iz/formats/bmp.html
 *		http://www.root.cz/clanky/bmp-na-vicero-zpusobu/
 *		https://en.wikipedia.org/wiki/BMP_file_format
 *		https://msdn.microsoft.com/en-us/library/windows/desktop/dd183391%28v=vs.85%29.aspx !!
 *		http://www.dragonwins.com/domains/getteched/bmp/bmpfileformat.htm !!
 *
 *	BMP consist of:
 *		"BM" (magic constants)
 *		Total size
 *		Reserved1
 *		Reserved2
 *		Offset bits
 *		Header size
 *		Header (Old if size == 12. If size > 12 its Microsoft Windows BMP file, we will use in this project):
 *			- Width
 *			- Height
 *			- Planes
 *			- Bits per Pixel
 *			- Compression
 *			- Image size
 *			- X pixels per meter
 *			- Y pixels per meter
 *			- Number of Colors
 *			- Colors Important
 *		Image data
 *
 * 	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param	bmp stucture for saving all information about the final BMP file.
 * 
 * 	@return None.
 */
void fillBMPdata(tGIF* gif, tBMP* bmp)
{
	bmp->signature = 0x4D42; // "BM" in hexa

	vector<BYTE> imageData = processBmpImgData(gif, bmp); // Decoded GIF data -> ordered RGB data for BMP
	bmp->bmpSize = 14 + 40 + imageData.size();

	bmp->reserved1 = 0x0000;	// Default values
	bmp->reserved2 = 0x0000;

	bmp->offset = 0x28;			// From the beginning of BMP header to the image data bits. 
								// Item headerSize (4 bytes) + header (36 bytes) = 40 decimal (0x28)

	bmp->headerSize = 0x28;		// The same as offset.
	bmp->header.width = gif->logicScreen.width;  // Image width and height in pixels.
	bmp->header.height = gif->logicScreen.height;

	bmp->header.planes = 0x01;				// Default value
	bmp->header.bitsPerPixel = 0x18; 		// Each pixel has RGB, each 1 BYTE, which means 24bits (0x18 hexa) - 16M colors.
	bmp->header.compression = 0x00; 		// Not compressed.
	bmp->header.imgSize = imageData.size();

	bmp->header.XPixelsPerMeter = 0x0000;	// They are just a preferrence to the display device. 0 means no preferrence.
	bmp->header.YPixelsPerMeter = 0x0000;

	bmp->header.numberOfColors = 0x00;		// Usually 0 (our case) except for indexed images using 
											// fewer than the max. number of colors in the Color Table.

	bmp->header.colorsImportant = 0x00;		// All colors from Color Table are important.

	bmp->imageData.clear();

	DEBUG_LOG(fprintf(stderr, "\n --- The final BMP data --- \n"));
	for(uint32_t i = 0; i < imageData.size(); i++) { // Final BMP data which will be on output.
		bmp->imageData.push_back(imageData[i]);
		DEBUG_LOG(fprintf(stderr, "%d ", imageData[i]));
	}
	DEBUG_LOG(fprintf(stderr,"\n\n"));
}


/**
 *	@synopsis	Function transforms already decoded GIF image data into interlaced data.
 *				Final result is ordered and prepared for later processing (RGB, ordering, data align).
 *				Source: https://www.w3.org/Graphics/GIF/spec-gif89a.txt Appendix E - Interlaced Images.
 *
 *	@param	width logical screen width of the input GIF file.
 *	@param	imageData vector containing BYTE values of GIF image data.
 *
 *	@return	None.
 */
void interlaceOrder(uint16_t width, vector<uint8_t>& imageData)
{
	vector<uint8_t> imageDataTmp; 	// Vector, data for interlacing.
	uint32_t tmpIndex = 0; 			// For indexing to vector containing original NOT interlaced data.

	for(uint32_t i = 0; i < imageData.size() ; i++)
		imageDataTmp.push_back(0);

	// Every 8 row, starting at row 0. 
	// Const '7' because there is another cycle inside, which fills exactly 1 row, therefore increase by 8-1.
	for(uint32_t i = 0; i < imageData.size() ; i += 7 * width) {
		for(uint8_t j = 0; j < width; j++) // Fill one selected row.
			imageDataTmp[i++] = imageData[tmpIndex++];
	}

	// Every 8 row, starting at row 4. 
	// Const '7' because there is another cycle inside, which fills exactly 1 row, therefore increase by 8-1.
	for(uint32_t i = 4 * width; i < imageData.size(); i += 7 * width) {
		for(uint8_t j = 0; j < width; j++) // Fill one selected row.
			imageDataTmp[i++] = imageData[tmpIndex++];
	}

	// Every 4 row, starting at row 2. 
	// Const '3' because there is another cycle inside, which fills exactly 1 row, therefore increase by 4-1.
	for(uint32_t i = 2 * width; i < imageData.size(); i += 3 * width) {
		for(uint8_t j = 0; j < width; j++) // Fill one selected row.
			imageDataTmp[i++] = imageData[tmpIndex++];
	}

	// Every 2 row, starting at row 1. 
	// Const '1' because there is another cycle inside, which fills exactly 1 row, therefore increase by 2-1.
	for(uint32_t i = 1 * width; i < imageData.size(); i += 1 * width) {
		for(uint8_t j = 0; j < width; j++) // Fill one selected row.
			imageDataTmp[i++] = imageData[tmpIndex++];
	}

	// Clear the original vector and fill him back with interlaced values.
	imageData.clear();

	for(uint32_t i = 0; i < imageDataTmp.size(); i++)
		imageData.push_back(imageDataTmp[i]);
}


/**
 *	@synopsis	Function transforms already decoded GIF image data into BMP image data.
 *				Final result is ordered and aligned as well.
 *
 *	@param	gif structure which contains all GIF data from the input GIF file. 
 *	@param 	bmp stucture for saving all information about the final BMP file.
 *
 *	@return	vector of WORDS which contains RGB image data already aligned and ordered according to the specification.
 */
vector<uint8_t> processBmpImgData(tGIF* gif, tBMP* bmp)
{
	if(gif->imgDescr[0].packetFields.interlaceFlag == 1) 
		interlaceOrder(gif->logicScreen.width, bmp->imageData);

	vector<uint8_t> imageData, orderedImageData;
	uint8_t align = (gif->logicScreen.width*3) % 4;

	if(gif->logicScreen.packetFields.GCTFlag == 1) {
		// The order is Blue Green Red as from the specification
		for(uint32_t i = 0; i < bmp->imageData.size(); i++) {
			imageData.push_back(gif->GCT[bmp->imageData[i]].blue);
			imageData.push_back(gif->GCT[bmp->imageData[i]].green);
			imageData.push_back(gif->GCT[bmp->imageData[i]].red);
		}
	}
	else {
		for(uint32_t i = 0; i < bmp->imageData.size(); i++) {
			imageData.push_back(gif->LCT[bmp->imageData[i]].blue);
			imageData.push_back(gif->LCT[bmp->imageData[i]].green);
			imageData.push_back(gif->LCT[bmp->imageData[i]].red);
		}
	}

	for(int32_t i = (gif->logicScreen.height - 1) * gif->logicScreen.width * 3; i >= 0;) {
		// Data order - first line in GIF is the last one in BMP and so on...
		for(uint32_t j = 0; j < gif->logicScreen.width*3; j++)
			orderedImageData.push_back(imageData[i+j]);

		// Data align, every line has to be aligned to 4 bytes.
		if(align != 0)
			for(uint8_t k = 0; k < (4 - align); k++)
				orderedImageData.push_back(0x00);

		i -= gif->logicScreen.width * 3; // Decrement, index to a previous line.
	}

	return orderedImageData;
}


/**
 *	@synopsis	Function basically outputs BMP structure to the output file/stream and creates BMP file.
 *
 *	@param 	bmp structure which contains all necessary  BMP data. 
 *	@param 	outputFile pointer to already opened output file, or stream if needed.
 *
 *	@return	None.
 */
void createBMPfile(tBMP* bmp, FILE *outputFile)
{
	uint8_t *tmpPtrBYTE;
	uint16_t *tmpPtrWORD;
	uint32_t *tmpPtrDWORD;

	// BMP
	tmpPtrWORD = &(bmp->signature);
	fwrite(tmpPtrWORD, sizeof(*tmpPtrWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->bmpSize);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrWORD = &(bmp->reserved1);
	fwrite(tmpPtrWORD, sizeof(*tmpPtrWORD), 1, outputFile);

	tmpPtrWORD = &(bmp->reserved2);
	fwrite(tmpPtrWORD, sizeof(*tmpPtrWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->offset);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->headerSize);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	// BMP header
	tmpPtrDWORD = &(bmp->header.width);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);
	
	tmpPtrDWORD = &(bmp->header.height);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrWORD = &(bmp->header.planes);
	fwrite(tmpPtrWORD, sizeof(*tmpPtrWORD), 1, outputFile);

	tmpPtrWORD = &(bmp->header.bitsPerPixel);
	fwrite(tmpPtrWORD, sizeof(*tmpPtrWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.compression);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.imgSize);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.XPixelsPerMeter);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.YPixelsPerMeter);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.numberOfColors);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	tmpPtrDWORD = &(bmp->header.colorsImportant);
	fwrite(tmpPtrDWORD, sizeof(*tmpPtrDWORD), 1, outputFile);

	// BMP image data
	for(uint32_t i = 0; i < bmp->imageData.size(); i++) {
		tmpPtrBYTE = &(bmp->imageData[i]);
		fwrite(tmpPtrBYTE, sizeof(*tmpPtrBYTE), 1, outputFile);
	}
}
