// QR_Code_Reader_Cpp.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>

#include "file_reader.h"

int main(int argc, char** argv)
{
	std::string bmpFileName = "test_picture.bmp";
	if (argc == 2) {
		bmpFileName = argv[1];
	}
	//Proposed work flow

	//1. Get access to picture data
	//1a. To begin with, read data from a file
	//TODO 1b. Read from other sources, such as camera
	BitmapImage image = File_Reader::Get_QR_Array(bmpFileName.c_str());
	
	for (int i = 0; i < image.Height; ++i) {
		for (int j = 0; j < image.Width; ++j) {
			cout << image.MonochromeImageData[i][j] << " ";
		}
		cout << std::endl;
	}
	
	//2 Use algorithm to parse meaning from picture

	//3 Present user with outcome

	// To stop the application from exiting immediately
	std::getchar();
	return 0;
}

