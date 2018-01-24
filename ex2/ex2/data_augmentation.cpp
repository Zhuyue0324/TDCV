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
	return u < n || v < n || w < n || u > x || v > x || w > x // ratio check
		;

}

void augment(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath) {
	Mat img = imread(inputpath + "0" + inputfilename + ".jpg");
	Mat RM;
	Mat output;
	float ran, ran1, ran2, ran3, ran4, ran5, ran6;
	vector<Point2f> pts1;
	vector<Point2f> pts2;
	int nbexamples = 10;

	imwrite(outputpath + inputfilename + "0.jpg", img);

	for (int i = 1; i < nbexamples; i++) {
		output = img.clone();
		//randomly flip
		ran = my_random(4);
		if (ran > 3) {
			flip(output, output, 0); //upside down
		}
		else if (ran > 2) {
			flip(output, output, 1); //left-right
		}
		else if (ran > 1) {
			flip(output, output, -1); //both, equivalent to a 180 rotation
		}
		//random rotation
		ran = my_random(360);
		RM = getRotationMatrix2D(Point(int(output.cols / 2), int(output.rows / 2)), int(ran), 1);
		warpAffine(output, output, RM, Point((int(output.cols), int(output.rows))), 1, cv::BORDER_REPLICATE, cv::Scalar(255, 255, 255));

		//randomly shearing

		ran = my_random(2);
		for (; ran > 1;) {
			ran1 = my_random(int(output.cols - 10)) + 5;//between 5 and cols-5
			ran2 = my_random(int(output.cols - 10)) + 5;
			ran3 = my_random(int(output.cols - 10)) + 5;
			ran4 = my_random(int(output.rows - 10)) + 5;
			ran5 = my_random(int(output.rows - 10)) + 5;
			ran6 = my_random(int(output.rows - 10)) + 5;
			pts1 = { Point2f(int(ran1), int(ran4)),
				Point2f(int(ran2), int(ran5)) ,
				Point2f(int(ran3), int(ran6)) };
			pts2 = { Point2f(int(ran1 - 5 + my_random(10)), int(ran4 - 5 + my_random(10))),
				Point2f(int(ran2 - 5 + my_random(10)), int(ran5 - 5 + my_random(10))) ,
				Point2f(int(ran3 - 5 + my_random(10)), int(ran6 - 5 + my_random(10))) };
			if (!invalid(pts1) && !invalid(pts2)) {
				RM = getAffineTransform(pts1, pts2);
				warpAffine(output, output, RM, Point((int(output.cols), int(output.rows))), 1, cv::BORDER_REPLICATE, cv::Scalar(255, 255, 255));
				break;
			}			
		}
		

		//random warpperception
		ran = my_random(10);
		pts1 = { Point2f(int(ran), int(ran)),
			Point2f(int(output.cols - ran), int(ran)) ,
			Point2f(int(ran / 2), int(output.rows - ran / 2)) ,
			Point2f(int(output.cols - ran / 2), int(output.rows - ran / 2)) };
		pts2 = { Point2f(0,0),
			Point2f(int(output.cols), 0) ,
			Point2f(0, int(output.rows)) ,
			Point2f(int(output.cols), int(output.rows)) };
		ran = my_random(3);
		if (ran >= 2) {
			RM = getPerspectiveTransform(pts1, pts2);//become bigger
			warpPerspective(output, output, RM, 
				Point((int(output.cols), int(output.rows))), 1, 
				cv::BORDER_REPLICATE, cv::Scalar(255, 255, 255));
		}
		else if (ran >= 1) {
			RM = getPerspectiveTransform(pts2, pts1);//become smaller
			warpPerspective(output, output, RM, 
				Point((int(output.cols), int(output.rows))), 1, 
				cv::BORDER_REPLICATE, cv::Scalar(255, 255, 255));
		}


		//output
		imwrite(outputpath + inputfilename + to_string(i) + ".jpg", output);
		

	}
}

void augment_MC(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath) {
	// Augment with Monte-Carlo strategy ==> abandoned
	Mat img = imread(inputpath + "0" + inputfilename + ".jpg");
	Mat RM;
	Mat output;
	int nbexamples = 10;

	imwrite(outputpath + inputfilename + "0.jpg", img);

	
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
	
		imwrite(outputpath + inputfilename + to_string(i) + ".jpg", output);
	}
}

int data_aug(string firstpart, bool verbose) {
	string lastpart = "train/0";
	vector<int> nums = { 53, 81, 51, 290 };
	string inputfilename;
	for (int i = 0; i < 4; i++) {
		string inputpath = firstpart + lastpart + to_string(i) + "/";
		string outputpath = firstpart + "aug/" + lastpart + to_string(i) + "/";
		if (verbose) {
			cout << "FOLDER " << lastpart << i << "\t\t\t\t\t\t" << endl;
		}
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
				cout << "  augmenting " << inputfilename + ".jpg\r" << flush;
			}
			augment(inputfilename, inputpath, outputpath);
		}
	}
	return 0;
}