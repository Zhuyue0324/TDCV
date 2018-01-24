
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

/*
Functions to augment the data, called from the main in mode 0
Folder hierarchy need to at least contain:
[Mother of folder we want to augment]
|-train [folder we want to augment]
|  |-00
|  |-01
|  |-02
|  `-03
`-aug
   `-train
      |-00
      |-01
      |-02
      `-03
*/

float my_random(int N);
void augment(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath);
void augment_MC(const cv::String &inputfilename, const cv::String &inputpath, const cv::String &outputpath);
int data_aug(string firstpart, bool verbose);
