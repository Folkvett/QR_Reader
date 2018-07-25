#include "file_reader.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
//Move includes from .h-file once obvious which can be moved

//Start with the most trivial picture, each pixel represent an entire cell in the QR code.

template <typename T>
T ReadDWord(char* buffer) {
	T readWord = ((T)buffer[0]
		+ ((T)(buffer[1]) << 8)
		+ ((T)(buffer[2]) << 16)
		+ ((T)(buffer[3]) << 24));

	return readWord;
}

template <typename T>
T ReadWord(char* buffer) {
	T readWord = ((T)buffer[0]
		+ ((T)(buffer[1]) << 8));

	return readWord;
}

BitmapImage& BitmapImage::operator=(const BitmapImage& other) {
	this->~BitmapImage();
	new (this) BitmapImage(other);
	return *this;
}

BitmapImage::~BitmapImage()
{
	for (int i = 0; i < Height; ++i) {
		delete[] ImageData[i];
		delete[] MonochromeImageData[i];
	}
	delete[] ImageData;
	delete[] MonochromeImageData;
}

BitmapImage::BitmapImage(BitmapImage && other) : 
	HeaderSize(other.HeaderSize),
 Height(other.Height),
 Width(other.Width),
	NumberOfColorplanes(other.NumberOfColorplanes),
	BitsPerPixel(other.BitsPerPixel),
	Compression(other.Compression),
	ImageSize(other.ImageSize),
	HorizontalResolution(other.HorizontalResolution),
	VerticalResolution(other.VerticalResolution),
	NumberOfColorsInColorPalette(other.NumberOfColorsInColorPalette),
	NumberOfImportantColorsInColorPalette(other.NumberOfImportantColorsInColorPalette)
{
	for (int i = 0; i < Height; ++i) {
		ImageData[i] = other.ImageData[i];
		MonochromeImageData[i] = other.MonochromeImageData[i];

		other.ImageData[i] = nullptr;
		other.MonochromeImageData[i] = nullptr;
	}
	ImageData = other.ImageData;
	MonochromeImageData = other.MonochromeImageData;

	other.ImageData = nullptr;
	other.MonochromeImageData = nullptr;
}

namespace File_Reader {

	BitmapImage Get_QR_Array(string filename) {
		//Actually not quite trivial, must know how data is stored and read bytes (?) 
		//Open file
		ifstream dataFile(filename.c_str(), ios::binary);
		BitmapImage bitmapImage;

		if (dataFile.is_open()) {
			// Read header
			char* headerInfo = new char[2];
			dataFile.read(headerInfo, 2);

			if (headerInfo[0] == 'B'
				&& headerInfo[1] == 'M') {
				cout << "Small success";
			}

			char* headerSize = new char[4];
			char* bufferForHeader = new char[4];
			dataFile.read(headerSize, 4);

			if (headerSize[0] != 0
				|| headerSize[1] != 0
				|| headerSize[2] != 0
				|| headerSize[3] != 0) {
				cout << "\nDid not open an empty file!";

				cout << headerSize[0] << ", " << headerSize[1] << ", " << headerSize[2] << ", " << headerSize[3] << std::endl;

				// Get rid of extra info for now
				char* headerExtraInfo = new char[4];
				dataFile.read(headerExtraInfo, 4);

				//Get offset for bitmap start
				char* headerBitdataStart = new char[4];
				dataFile.read(headerBitdataStart, 4);

				// wtf
				//cout << "Data starts at: " << _itoa_s((int)headerSize[3], bufferForHeader, 4, 1);

				// Read DIB Data header
				// First char is LSW
				char* DIBSize = new char[4];
				dataFile.read(DIBSize, 4);

				// Mash all bits together into one integer
				long int DIBSizeAsInt = ReadDWord<int>(DIBSize);
				cout << "Dib size (used to figure out heading): " << DIBSizeAsInt << std::endl;

				// 0x28 in hex, BITMAPINFOHEADER, most common for Windows NT and later
				if (DIBSizeAsInt == 40) {
					dataFile.seekg(14);
					char* BitmapHeaderSize = new char[4];
					dataFile.read(BitmapHeaderSize, 4);

					long int BitmapHeaderSizeAsInt = ReadDWord<int>(BitmapHeaderSize);
					bitmapImage.HeaderSize = BitmapHeaderSizeAsInt;
					cout << "BitmapHeaderSize (used to figure out heading): " << BitmapHeaderSizeAsInt << std::endl;

					// Width
					char* BitmapHeaderPictureWidth = new char[4];
					dataFile.read(BitmapHeaderPictureWidth, 4);

					long int PictureWidth = ReadDWord<int>(BitmapHeaderPictureWidth);
					bitmapImage.Width = PictureWidth;
					cout << "Width of picture: " << PictureWidth << std::endl;

					// Height
					char* BitmapHeaderPictureHeight = new char[4];
					dataFile.read(BitmapHeaderPictureHeight, 4);

					long int PictureHeight = ReadDWord<int>(BitmapHeaderPictureHeight);
					bitmapImage.Height = PictureHeight;
					cout << "Height of picture: " << PictureHeight << std::endl;

					// Colorplanes 2 bits
					char* BitmapHeaderColorPlanes = new char[2];
					dataFile.read(BitmapHeaderColorPlanes, 2);

					long int Colorplanes = ReadWord<int>(BitmapHeaderColorPlanes);
					bitmapImage.NumberOfColorplanes = Colorplanes;
					cout << "Colorplanes: " << Colorplanes << std::endl;

					// bits per pixel (bpp) 2 bits
					char* BitmapHeaderBPP = new char[2];
					dataFile.read(BitmapHeaderBPP, 2);

					long int BPP = ReadWord<int>(BitmapHeaderBPP);
					bitmapImage.BitsPerPixel = BPP;
					cout << "Bits Per Pixel: " << BPP << std::endl;

					//Compression type 4 bits
					char* BitmapHeaderCompression = new char[4];
					dataFile.read(BitmapHeaderCompression, 4);

					long int Compression = ReadWord<int>(BitmapHeaderCompression);
					bitmapImage.Compression = Compression;
					cout << "Compression : " << Compression << std::endl;

					//image size 4 bits
					char* BitmapHeaderImageSize = new char[4];
					dataFile.read(BitmapHeaderImageSize, 4);

					long int ImageSize = ReadWord<int>(BitmapHeaderImageSize);
					bitmapImage.ImageSize = ImageSize;
					cout << "ImageSize : " << ImageSize << std::endl;

					// Horizontal resolution (pixel per meter) 4 bits
					char* BitmapHeaderHorizontalResolution = new char[4];
					dataFile.read(BitmapHeaderHorizontalResolution, 4);

					long int HorizontalResolution = ReadWord<int>(BitmapHeaderHorizontalResolution);
					bitmapImage.HorizontalResolution = HorizontalResolution;
					cout << "HorizontalResolution : " << HorizontalResolution << std::endl;

					// Vertical resolution (pixel per meter) 4 bits
					char* BitmapHeaderVerticalResolution = new char[4];
					dataFile.read(BitmapHeaderVerticalResolution, 4);

					long int VerticalResolution = ReadWord<int>(BitmapHeaderVerticalResolution);
					bitmapImage.VerticalResolution = VerticalResolution;
					cout << "VerticalResolution : " << VerticalResolution << std::endl;

					// Number of colors in color palette 4 bits
					char* BitmapHeaderNumberofColor = new char[4];
					dataFile.read(BitmapHeaderNumberofColor, 4);

					long int NumberofColors = ReadWord<int>(BitmapHeaderNumberofColor);
					bitmapImage.NumberOfColorsInColorPalette = NumberofColors;
					cout << "NumberofColors : " << NumberofColors << std::endl;

					// Number of important colors, usually ignored 4 bits
					char* BitmapHeaderNumberOfImportantColor = new char[4];
					dataFile.read(BitmapHeaderNumberOfImportantColor, 4);

					long int NumberOfImportantColor = ReadWord<int>(BitmapHeaderNumberOfImportantColor);
					bitmapImage.NumberOfImportantColorsInColorPalette = NumberOfImportantColor;
					cout << "NumberOfImportantColor : " << NumberOfImportantColor << std::endl;

					//Finally! Begin reading image data
					if (bitmapImage.BitsPerPixel == 1) {
						//Assume for now: Each pixel is black or white

						// Set up bitmap image
						bitmapImage.ImageData = new Pixel*[bitmapImage.Height];
						for (int i = 0; i < bitmapImage.Height; ++i) {
							bitmapImage.ImageData[i] = new Pixel[bitmapImage.Width];
						}

						// And the simpler matrix as well
						bitmapImage.MonochromeImageData = new int*[bitmapImage.Height];
						for (int i = 0; i < bitmapImage.Height; ++i) {
							bitmapImage.MonochromeImageData[i] = new int[bitmapImage.Width];
						}

						// Start reading from bottom left, must loop Height number of times. 
						// Width must be divisible by 4, otherwise padding is added (which may not be 0)

						//since each bit represents a pixel
						// assume that there are no new rows that begin in the same byte as another ends in
						// MSB to LSB encoding (little endian)

						// Add extra width for the padding
						// Incorrect calculation for this case! This is good for the general case but here the width is in pixels, which is
						// the same as bits, but adding 32 should solve this? If all padding is based on DWORDS (32 bits)
						int paddedWidth = bitmapImage.Width + (bitmapImage.Width % 32); 

						char* buffer = new char[1];
						for (int row = 0; row < bitmapImage.Height; ++row) {
							for (int column = 0; column < paddedWidth; column += 8) {
								// must check if each bit is actually part of the image or if it is padded
								dataFile.read(buffer, 1);
								for (int bitIndex = 0; bitIndex < 8; ++bitIndex) {
									if (column + bitIndex < bitmapImage.Width) {
										bitmapImage.ImageData[row][column + bitIndex].Red = (((*buffer) >> bitIndex) & 0x01) * 255;
										bitmapImage.ImageData[row][column + bitIndex].Blue = (((*buffer) >> bitIndex) & 0x01) * 255;
										bitmapImage.ImageData[row][column + bitIndex].Green = (((*buffer) >> bitIndex) & 0x01) * 255;

										bitmapImage.MonochromeImageData[row][column + bitIndex] = (((*buffer) >> bitIndex) & 0x01);
									} else {
										cout << "Padding \n";
									}
								}
							}							
						}
						for (int i = 0; i < bitmapImage.Height; ++i) {
							for (int j = 0; j < bitmapImage.Width; ++j) {
								cout << bitmapImage.MonochromeImageData[i][j] << " ";
							}
							cout << std::endl;
						}

					}
					else if (bitmapImage.BitsPerPixel == 8) {

					}
					// bitmaps are stored with the leftmost lower corner first, as opposed to upper corner where the QR reader will begin
					// to "scan" the image. flip the picture height-wise to prepare the image for the scanner.
					for (int i = bitmapImage.Height - 1; i > (bitmapImage.Height / 2); --i) {
						std::swap(bitmapImage.ImageData[i], bitmapImage.ImageData[bitmapImage.Height - i - 1]);
						std::swap(bitmapImage.MonochromeImageData[i], bitmapImage.MonochromeImageData[bitmapImage.Height - i - 1]);
					}
				}
			}

		}

		//Read file contents OR
		//Convert into suitable data?


		dataFile.close();
		//return result

		return bitmapImage;
	}
}

BitmapImage::BitmapImage() :
	HeaderSize(0),
	Height(0),
	Width(0),
	NumberOfColorplanes(0),
	BitsPerPixel(0),
	Compression(0),
	ImageSize(0),
	HorizontalResolution(0),
	VerticalResolution(0),
	NumberOfColorsInColorPalette(0),
	NumberOfImportantColorsInColorPalette(0),
	ImageData(nullptr),
	MonochromeImageData(nullptr) {
}

BitmapImage::BitmapImage(const BitmapImage& other)
{
	HeaderSize = other.HeaderSize;
	Height = other.Height;
	Width = other.Width;
	NumberOfColorplanes = other.NumberOfColorplanes;
	BitsPerPixel = other.BitsPerPixel;
	Compression = other.Compression;
	ImageSize = other.ImageSize;
	HorizontalResolution = other.HorizontalResolution;
	VerticalResolution = other.VerticalResolution;
	NumberOfColorsInColorPalette = other.NumberOfColorsInColorPalette;
	NumberOfImportantColorsInColorPalette = other.NumberOfImportantColorsInColorPalette;

	ImageData = new Pixel*[other.Height];
	for (int i = 0; i < other.Height; ++i) {
		ImageData[i] = new Pixel[other.Width];
	}

	MonochromeImageData = new int*[other.Height];
	for (int i = 0; i < other.Height; ++i) {
		MonochromeImageData[i] = new int[other.Width];
	}
}
