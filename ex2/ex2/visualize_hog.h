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

void visualizeHOG(cv::Mat img, std::vector<float> &feats, cv::HOGDescriptor hog_detector, int scale_factor = 3);
/*
* img - the image used for computing HOG descriptors. **Attention here the size of the image should be the same as the window size of your cv::HOGDescriptor instance **
* feats - the hog descriptors you get after calling cv::HOGDescriptor::compute
* hog_detector - the instance of cv::HOGDescriptor you used
* scale_factor - scale the image *scale_factor* times larger for better visualization
*/