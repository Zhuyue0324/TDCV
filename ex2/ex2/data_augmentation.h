
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

float my_random(int N);
void augment(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath);
void augment_MC(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath);
int data_aug(string firstpart, bool verbose);
