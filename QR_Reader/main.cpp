// QR_Code_Reader_Cpp.cpp : Defines the entry point for the console application.
//

#include "file_reader.h"
#include <iostream>

int main()
{
	//Proposed work flow

	//1. Get access to picture data
	//1a. To begin with, read data from a file
	//TODO 1b. Read from other sources, such as camera
	BitmapImage image = File_Reader::Get_QR_Array("test_picture.bmp");
	
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

