#ifndef file_reader_h
#define file_reader_h

#pragma once
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

//Instantiate F_R_C with file in ctor or supply static methods?'
//If this is a utility class with "atomic" function, static would be the prefered 
// method

//Test message to check how to create commits in git.

struct Pixel {
	int Red;
	int Green;
	int Blue;
	int Alpha;
};

struct BitmapImage {
	BitmapImage();
	BitmapImage(const BitmapImage& other);
	BitmapImage& operator=(const BitmapImage& other);
	~BitmapImage();
	BitmapImage(BitmapImage&& other);

	int HeaderSize;
	int Height;
	int Width;
	int NumberOfColorplanes;
	int BitsPerPixel;
	int Compression;
	int ImageSize;
	int HorizontalResolution;
	int VerticalResolution;
	int NumberOfColorsInColorPalette;
	int NumberOfImportantColorsInColorPalette;
	Pixel** ImageData;
	int** MonochromeImageData; // A simplified ImageData, useful for the QR reader
};

//If this is only a collection of functions, why create a class at all? Use namespace and collect free functions? Reduce
//overhead from object orientation and easier to test
namespace File_Reader {
	BitmapImage Get_QR_Array(string filename);
}

#endif // !file_reader_h
