#include "data_augmentation.h"
#include "stdafx.h"
#include <opencv2/opencv.hpp> 
#include <opencv2/objdetect.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <stdio.h>
#include <stddef.h>

using namespace cv;
using namespace std;

//take a random value between 0 and N (incluse)
float my_random(int N) {
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / N));
	
}

// says whether a triangle is not normal enough to be warped into
const bool invalid(const vector<Point2f> tri) {

	// we only want the ratios between the three edges to be close enough to 1
	const float x = 1.4; // maxratio
	const float n = 0.8; // minratio
	const float a = norm(tri[0] - tri[1]);
	const float b = norm(tri[1] - tri[2]);
	const float c = norm(tri[0] - tri[2]);
	const float u = a / b;
	const float v = b / c;
	const float w = c / a;
	return u < n || v < n || w < n || u > x || v > x || w > x;

}

void augment(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath) {
	Mat img = imread(inputpath + "0" + inputfilename + ".jpg");
	Mat RM;
	Mat output;
	//float ran, ran1, ran2, ran3, ran4, ran5, ran6;
	//vector<Point2f> pts1;
	//vector<Point2f> pts2;
	int nbexamples = 10;

	imwrite(outputpath + inputfilename + "00.jpg", img);

	
	for (int i = 1; i < nbexamples; i++) {
		output = img.clone();
		
		// random image of the base
		vector<Point2f> goal(3);
		vector<Point2f> base(3);
		goal[0] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		goal[1] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		goal[2] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		base[0] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		base[1] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		base[2] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		for (;invalid(goal)||invalid(base);) { // it has to be a valid triangle
			goal[0] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
			goal[1] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
			goal[2] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
			base[0] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
			base[1] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
			base[2] = Point2f(5 + rand() % (output.cols - 10), 5 + rand() % (output.rows - 10));
		}

		RM = getAffineTransform(base, goal);
		warpAffine(output, output, RM, Point((int(output.cols), int(output.rows))), 1,
			cv::BORDER_REPLICATE, cv::Scalar(255, 255, 255));
	/*
		circle(output, base[0], 2, Scalar(0, 255, 0));
		circle(output, base[1], 2, Scalar(0, 255, 0));
		circle(output, base[2], 2, Scalar(0, 255, 0));
		circle(output, goal[0], 2, Scalar(0, 0, 255));
		circle(output, goal[1], 2, Scalar(0, 0, 255));
		circle(output, goal[2], 2, Scalar(0, 0, 255));
	*/	
		//output
		if (i < 10) {
			imwrite(outputpath + inputfilename + "0" + to_string(i) + ".jpg", output);
		}
		else {
			imwrite(outputpath + inputfilename + to_string(i) + ".jpg", output);
		}

	}
}

int data_aug(string firstpart, bool verbose) {
	string lastpart = "train/0";
	vector<int> nums = { 53, 81, 51, 290 };
	string inputfilename;
	for (int i = 0; i < 4; i++) {
		string inputpath = firstpart + lastpart + to_string(i) + "/";
		string outputpath = firstpart + "aug_" + lastpart + to_string(i) + "/";
		for (int j = 0; j < nums[i]; j++) {
			if (j < 10) {
				inputfilename = "00" + to_string(j);
			}
			else if (j < 100) {
				inputfilename = "0" + to_string(j);
			}
			else {
				inputfilename = "" + to_string(j);
			}
			if (verbose) {
				cout << " augmenting " << inputfilename + ".jpg" << endl;
			}
			//imshow("test", imread(inputpath+inputfilename + ".jpg", 1));
			augment(inputfilename, inputpath, outputpath);
		}
	}
	return 0;
}