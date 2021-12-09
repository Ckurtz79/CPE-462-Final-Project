#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <random>
// # of functions is: 8
using namespace cv; //histogramEQ returns int **output which replaces image_out
int **histogramEQ(const int &width, const int &height, int **input, int **output){ //could have passed a reference to output but having a variable with a type int **& was slightly worrying
	int hist[256] = {0};
	int newGreyLevels[256] = {0};
	int curr = 0;
	long loopRuns = 0; //used for testing algorithmic efficiency, it is not efficient at all and loops over 1.5M times
	// for a small JPG, not sure what other structure could make this faster since most of the work here is initialization
	for (int i = 0; i < width; i++){ 
		for (int j = 0; j <height; j++){
			loopRuns++;
			hist[input[j][i]]++; //finding frequency of intensities in image
		}
	}
	for (int i = 0; i < 256; i++){
		loopRuns++;
		curr += hist[i];
		newGreyLevels[i] = round((((float)curr) *255) / (width*height)); //use of round and float is probably redundant but somewhat more accurate
	}

	for (int row = 0; row < width; row++){
		for (int col = 0; col < height; col++){
			loopRuns++;
			output[col][row] = newGreyLevels[input[col][row]];
			//mapping new grey levels to the output image, apparently the position of col and row are *very* important and cannot be reversed
		}
	}
	return output;
	//these nested loops could very well be multithreaded but im not sure what kind of mutex implementation it would require 
}
void negative(const int &width, const int &height, int** input, int**& output){
	int j,k;
	for (j=0; j<height; j++){
		for (k=0; k<width; k++){
			output[j][k]=255-input[j][k]; //produces a negative of the image
		}
	}
}
int **sobelEdgeDetection(const int &width, const int &height, int** input, int** output){
	int sum = 0;
	int fgx;
	int fgy;
	for(int y = 1; y < height - 1; y++){
            for(int x = 1; x < width - 1; x++){
                auto gy = [](int **input, int y, int x) -> int {return (input[y-1][x-1] + 2*input[y-1][x] + input[y-1][x+1] - input[y+1][x-1] -
                   2*input[y+1][x] - input[y+1][x+1]);}; //there is probably unecessary padding in these functions for type safety
				auto gx = [](int **input, int y, int x) -> int {return (input[y-1][x-1] + 2*input[y][x-1] + input[y+1][x-1] - input[y-1][x+1] -
                   2*input[y][x+1] - input[y+1][x+1]);};
				fgx = gx(input, y, x);
                fgy = gy(input, y, x);
                sum = abs(fgx) + abs(fgy);
                sum = sum > 255 ? 255:sum;
                sum = sum < 0 ? 0 : sum;
                output[y][x] = sum;
            }
        }

	return output;
}
int **prewittEdgeDetection(const int &width, const int &height, int** input, int** output){
	int sum = 0;
	int fgx;
	int fgy;
	for(int y = 1; y < height - 1; y++){
            for(int x = 1; x < width - 1; x++){
                auto gx = [](int **input, int y, int x) -> int {return (-1 * input[y-1][x-1] - input[y-1][x] - input[y-1][x+1] + input[y+1][x-1] +
                   input[y+1][x] + input[y+1][x+1]);}; 
				auto gy = [](int **input, int y, int x) -> int {return (-1 * input[y-1][x-1] - input[y][x-1] - input[y+1][x-1] + input[y-1][x+1] +
                   input[y][x+1] + input[y+1][x+1]);};
				fgx = gx(input, y, x);
                fgy = gy(input, y, x);
                sum = abs(fgx) + abs(fgy);
                sum = sum > 255 ? 255:sum;
                sum = sum < 0 ? 0 : sum;
                output[y][x] = sum;
            }
        }

	return output;
}
int **convolution(const int &width, const int &height, int **input, int **kernel, int** output){
	int convolve;
	int ii, jj, y, x;
	for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k< 3; k++) {
                y = 2 - k;
              for (int l = 0; l< 3; l++) {
				x = 2 - l;
				ii = i + (1 - y);
				jj = j + (1- x);
                if (jj >= 0 && jj < width && ii >= 0 && ii < height){
                   output[i][j] += (input[ii][jj] * kernel[y][x]);
				}
              }
            }
        }
    }
	for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
			output[i][j] = output[i][j] > 255 ? 255 : output[i][j];
			output[i][j] = output[i][j] < 0 ? 0 : output[i][j];
		}
	}
	return output;
}
int **thresholding(const int &width, const int &height, int** input, int** output){
	int threshold = 128; //starting the threshold at the middle of the range seems fair
	int oldThreshold;
	int deltaT = 5;
	uint64_t brightP = 0, darkP = 0, bSum = 0, dSum = 0; //probably need 64bit int and wont be negative ever
	double bAvg = 0, dAvg = 0;
	//use a do while to get into the loop before checking since we know we're going to be in it at least once
	do{
		oldThreshold = threshold;
		for (int j = 0; j < height; j++){
			for (int k = 0; k < width; k++) {
				if(input[j][k] >= threshold){
					bSum += input[j][k];
					brightP++;
				}else{
					dSum += input[j][k];
					darkP++;
				}
			}
		}
		bAvg = (double)bSum/brightP;
		dAvg = (double)dSum/darkP;
		threshold = (bAvg + dAvg) * 0.5;
		deltaT = abs(threshold - oldThreshold);
	} while(deltaT >= 5);

	for (int j = 0; j < height; j++){
		for (int k = 0; k < width; k++) {
			//std::cout << "thresholding" << std::endl;
			output[j][k] = input[j][k] >= threshold ? 255 : 0;
		}
	}

	return output;
}
int **addSpeckle(const int &width, const int &height, int** input, int **output){
	std::random_device rd{};
	std::mt19937 gen(rd()); //supposedly this will generate a normal distribution

	std::normal_distribution<double> dist{1.75, 0};
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++){
			output[i][j] = std::round(input[i][j] * dist(gen));
		}
	}
	return output;
}
int **whiteNoise(const int &width, const int &height, int** input, int **output){
	std::random_device rd{};
	std::mt19937 gen(rd()); //supposedly this will generate a normal distribution

	std::normal_distribution<double> dist{0, 40};
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++){
			output[i][j] = std::round(input[i][j] + dist(gen));
		}
	}	
	return output;
}

int main(int argc, char *argv[]){
	int j, k, width, height;
	std::string windowName = "Input Image";
	if(argc<5) { printf("ERROR: Insufficient parameters!\n"); return(1);}

	width = atoi(argv[3]);
	height = atoi(argv[4]);

	Mat M_in = imread(argv[1]);
	Mat_ <uchar> M_in_g(M_in.rows, M_in.cols);
	cvtColor(M_in, M_in_g, COLOR_BGR2GRAY);

	height = M_in_g.rows;
	width = M_in_g.cols;
/****************************************************************
 * Micellaneus memory allocation
 **************************************************************/
	int **kernel = new int*[3];
	for (int i = 0; i < 3; i++) {
		kernel[i] = new int[3];
	}
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			kernel[i][j] = -1;
		}
	}
	kernel[1][1] = 9;
	kernel[0][0] = kernel[0][2] = kernel[2][0] = kernel[2][2] = -1;

	int **image_in = new int*[height];
	if(!image_in){
		printf("Error: Can't allocate memmory!\n");
		return(1);
	}
	int **image_out = new int*[height];
	if(!image_out){
		printf("Error: Can't allocate memmory!\n");
		return(1);
	}
	for (j=0; j<height; j++){
		image_in[j] = new int[width];
		if(!image_in[j]){
			printf("Error: Can't allocate memmory!\n");
			return(1);
		}
		image_out[j] = new int[width];
		if(!image_out[j]){
			printf("Error: Can't allocate memmory!\n");
			return(1);
		}
	}
/****************************************************************
 * Micellaneus memory allocation
 **************************************************************/

	for (j=0; j<height; j++){
		for (k=0; k<width; k++){
			image_in[j][k] = M_in_g(j,k);
		}
	}

	Mat_ <uchar> M_out(height, width);
	int choice;
	std::cout << "Choose an Processing Function\n" << "Type 1 to output the negative of your image" << std::endl;
	std::cout << "Type 2 to output a histogram equalized version of your image\n" << "Type 3 to apply an edge detection mask on your image" << std::endl;
	std::cout << "Type 4 to apply a laplacian sharpening mask on your image\n" << "Type 5 to threshold image\n" << "Type 6 to apply Prewitt Edge Detection mask on your image" << std::endl;
	std::cin >> choice;
	switch(choice){
		case 1: 
			negative(width, height, image_in, image_out);
			break;
		case 2:
			image_out = histogramEQ(width, height, image_in,image_out);
			break;
		case 3:
			image_out = sobelEdgeDetection(width, height, image_in, image_out);
			break;
		case 4:
			image_out = convolution(width, height, image_in, kernel, image_out);
			break;
		case 5:
			image_out = thresholding(width, height, image_in, image_out);
			break;
		case 6:
			image_out = prewittEdgeDetection(width, height, image_in, image_out);
			break;
		case 7:
			image_out = addSpeckle(width, height, image_in, image_out);
			break;
		case 8:
			image_out = whiteNoise(width, height, image_in, image_out);
			break;
		default:
			return 0;
	}
	for (int ii = 0; ii < height; ii++)
		for (int jj = 0; jj < width; jj++)
			M_out (ii, jj) = image_out[ii][jj];
	std::string secondWindow = "output";
	namedWindow(secondWindow);
	namedWindow(windowName);
	imshow(secondWindow, M_out);
	imshow(windowName, M_in_g);
	waitKey(0);
	destroyWindow(secondWindow);
	destroyWindow(windowName);


	bool success = imwrite(argv[2], M_out);
	if (!success){
		std::cout << "save error"<< std::endl;
		return 6;
	}

	delete[] image_in;
	delete[] image_out;
	delete[] kernel;


    return 0;
}



