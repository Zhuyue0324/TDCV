// ex2.cpp : entrance point for console application

#include "stdafx.h"
#include <opencv2/opencv.hpp> 
#include <opencv2/objdetect.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "data_augmentation.h"
#include "visualize_hog.h"
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <stdio.h>
#include <stddef.h>

using namespace cv;
using namespace std;

// input number (for interactive mode)
int inputNumber(string msg) {
	int myNumber = 0;
	string input = "";
	while (true) {
		std::cout << msg;
		getline(cin, input);

		// This code converts from string to number safely.
		stringstream myStream(input);
		if (myStream >> myNumber)
			break;
		std::cout << "Invalid number, please try again" << endl;
	}
	return myNumber;
}

// =-=-=-=-= TASK 1 =-=-=-=-=


// main pipeline for task 1
int task1(const cv::String &filename)
{
	Mat img = imread(filename);
	if (img.empty()) // Check for invalid input
	{
		std::cout << "Could not open or find the image" << endl;
		return -1;
	}
	Mat img1;
	resize(img, img, Size(128, 104));
	cvtColor(img, img1, cv::COLOR_RGB2GRAY);
	Size winSize(128, 104);
	Size blockSize(32, 26);
	Size blockStride(16, 13);
	Size cellSize(16, 13);
	HOGDescriptor *hog = new HOGDescriptor(winSize, blockSize, blockStride, cellSize, 9);
	vector<float> descriptors;
	hog->compute(img1, descriptors, Size(2, 2), Size(0, 0));
	visualizeHOG(img, descriptors, *hog, 3);

	return 0;
}

// =-=-=-=-= TASK 2 =-=-=-=-=

// load imags in a given range
void loadImgs(Mat &data, Mat &label, HOGDescriptor *hog, const cv::String &folderName, const int beginIdx, const int endIdx, const int lbl) {
	string ii;
	string imgpath;
	for (int i = beginIdx; i < endIdx; i++) {
		if (i < 10)
			ii = "00" + to_string(i);
		else if (i<100)
			ii = "0" + to_string(i);
		else
			ii = to_string(i);

		imgpath = folderName + ii + ".jpg";
		Mat img = imread(imgpath);
		if (img.cols != 0) {
			resize(img, img, Size(128, 104));
			cvtColor(img, img, cv::COLOR_RGB2GRAY);
			vector<float> descriptors;
			hog->compute(img, descriptors, Size(2, 2), Size(0, 0));
			data.push_back(descriptors);
			label.push_back(lbl);
		}
		else {
			std::cout << "Could not read image " << imgpath << endl;
		}
	}
}

// load all images in a sequence, starting from 0
void loadImgs(Mat &data, Mat &label, HOGDescriptor *hog, const cv::String &folderName, const int lbl) {
	string ii;
	string imgpath;
	for (int i = 0;; i++) {
		if (i < 10)
			ii = "000" + to_string(i);
		else if (i<100)
			ii = "00" + to_string(i);
		else if (i<1000)
			ii = "0" + to_string(i);
		else
			ii = to_string(i);

		imgpath = folderName + ii + ".jpg";
		Mat img = imread(imgpath);
		if (img.cols != 0) {
			resize(img, img, Size(128, 104));
			cvtColor(img, img, cv::COLOR_RGB2GRAY);
			vector<float> descriptors;
			hog->compute(img, descriptors, Size(2, 2), Size(0, 0));
			data.push_back(descriptors);
			label.push_back(lbl);
		}
		else {
			std::cout << "Stopped at image " << imgpath << endl;
			break;
		}
	}
}

// load one image
void loadImg(Mat &data, Mat &label, HOGDescriptor *hog, const cv::String &imgpath, const int lbl) {
	Mat img = imread(imgpath);
	if (img.cols != 0) {
		resize(img, img, Size(128, 104));
		cvtColor(img, img, cv::COLOR_RGB2GRAY);
		vector<float> descriptors;
		hog->compute(img, descriptors, Size(2, 2), Size(0, 0));
		data.push_back(descriptors);
		label.push_back(lbl);
	}
	else {
		std::cout << "[ERROR] Couldn't read image " << imgpath << endl;
	}
}

// load train data
void loadTrain(Mat &traindata, Mat &trainlabel, HOGDescriptor *hog, const cv::String &folderName) {
	loadImgs(traindata, trainlabel, hog, folderName + "/00/0", 0, 49, 0);
	loadImgs(traindata, trainlabel, hog, folderName + "/01/0", 0, 67, 1);
	loadImgs(traindata, trainlabel, hog, folderName + "/02/0", 0, 42, 2);
	loadImgs(traindata, trainlabel, hog, folderName + "/03/0", 0, 53, 3);
	loadImgs(traindata, trainlabel, hog, folderName + "/04/0", 0, 67, 4);
	loadImgs(traindata, trainlabel, hog, folderName + "/05/0", 0, 110, 5);
}

// load test data
void loadTest(Mat &testdata, Mat &testlabel, HOGDescriptor *hog, const cv::String &folderName) {
	loadImgs(testdata, testlabel, hog, folderName + "/00/0", 49, 59, 0);
	loadImgs(testdata, testlabel, hog, folderName + "/01/0", 67, 77, 1);
	loadImgs(testdata, testlabel, hog, folderName + "/02/0", 42, 52, 2);
	loadImgs(testdata, testlabel, hog, folderName + "/03/0", 53, 63, 3);
	loadImgs(testdata, testlabel, hog, folderName + "/04/0", 67, 77, 4);
	loadImgs(testdata, testlabel, hog, folderName + "/05/0", 110, 120, 5);
}

// create a tree
Ptr<ml::DTrees> createTree(int cvf, int mc, int md, int msc) {
	Ptr<ml::DTrees> dtree = ml::DTrees::create();
	dtree->setCVFolds(cvf);
	dtree->setMaxCategories(mc);
	dtree->setMaxDepth(md);
	dtree->setMinSampleCount(msc);
	return(dtree);
}

// main pipeline for task 2
void task2(const cv::String folderName, const bool verbose, const bool interactive) {

	//variable initialization
	Mat traindata;
	Mat testdata;
	Mat trainlabel;
	Mat testlabel;

	// HOG parameters
	Size winSize(128, 104);
	Size blockSize(32, 26);
	Size blockStride(16, 13);
	Size cellSize(16, 13);
	HOGDescriptor *hog = new HOGDescriptor(winSize, blockSize, blockStride, cellSize, 9);

	//prepare the train data, train label, test data and test label and reshape them
	loadTrain(traindata, trainlabel, hog, folderName + "/train"); //388
	loadTest(testdata, testlabel, hog, folderName + "/test");  //60

	traindata.convertTo(traindata, CV_32F);
	traindata = traindata.reshape(1, 388);
	testdata.convertTo(testdata, CV_32F);
	testdata = testdata.reshape(1, 60);

	if (verbose) {
		std::cout << "Data loaded\n";
	}

	//Decision Tree part

	//Set parameters
	int cvf = 0;
	int mc = 2;
	int md = 6;
	int msc = 2;
	if (interactive) {
		int yesno = inputNumber("Want to change Decision Tree parameters? (yes=1)  ");
		if (yesno == 1) {
			cvf = inputNumber("Change CVFolds (default " + to_string(cvf) + "): ");
			mc = inputNumber("Change MaxCategories (default " + to_string(mc) + "): ");
			md = inputNumber("Change MaxDepth (default " + to_string(md) + "): ");
			msc = inputNumber("Change MinSampleCount (default " + to_string(msc) + "): ");
			std::cout << "Ok.\n";
		}
	}

	
	Ptr<ml::DTrees> dtree = createTree(cvf, mc, md, msc);
	Ptr<ml::TrainData> tData = ml::TrainData::create(traindata, ml::ROW_SAMPLE, trainlabel);

	// Train on traindata
	dtree->train(tData);

	// Analize response on the testdata
	Mat output;
	float response = dtree->predict(testdata, output);

	int counttree = 0;
	for (int i = 0; i < 60; i = i + 1) {
		if (int(output.at<float>(i, 0)) != int(testlabel.at<int>(i, 0))) {
			counttree = counttree + 1;
		}
	}
	std::cout << "The percentage of error with decision tree with depth "<<md<<" is "
		<< counttree * 100 / 60
		<< "%\n";

	//Forest part

	//Set parameters
	int nbtree = 8;
	if (interactive) {
		int yesno = inputNumber("Want to change Decision Forest parameters? (yes=1)  ");
		if (yesno == 1) {
			nbtree = inputNumber("Change nbTree (default " + to_string(nbtree) + "): ");
			cvf = inputNumber("Change CVFolds (default " + to_string(cvf) + "): ");
			mc = inputNumber("Change MaxCategories (default " + to_string(mc) + "): ");
			md = inputNumber("Change MaxDepth (default " + to_string(md) + "): ");
			msc = inputNumber("Change MinSampleCount (default " + to_string(msc) + "): ");
			std::cout << "Ok.\n";
		}
	}
	Mat resulttable = Mat::zeros(60, 6, CV_32F);
	
	// For each tree independently
	for (int i = 0; i < nbtree; i = i + 1) {
		//generate a random subset of the train data
		Mat subtraindata;
		Mat subtrainlabel;
		vector<int> generator(388);
		for (int i = 0; i < 388; i = i + 1) {
			generator[i] = i;
		}
		random_shuffle(generator.begin(), generator.end());
		for (int i = 0; i < 300; i = i + 1) {
			subtraindata.push_back(traindata.row(generator[i]));
			subtrainlabel.push_back(trainlabel.row(generator[i]));
		}

		// train a new tree on it
		Ptr<ml::DTrees> dtree = createTree(cvf, mc, md, msc);
		Ptr<ml::TrainData> tData = ml::TrainData::create(subtraindata, ml::ROW_SAMPLE, subtrainlabel);
		dtree->train(tData);

		// predict on the testdata and store the result
		float response = dtree->predict(testdata, output);
		for (int j = 0; j < 60; j = j + 1) {
			resulttable.at<float>(j, int(output.at<float>(j, 0))) = resulttable.at<float>(j, int(output.at<float>(j, 0))) + 1;
		}
	}
	// Compare ground truth with a majority vote from the forest
	Mat prediction = Mat::zeros(60, 3, CV_32F);
	for (int i = 0; i < 60; i = i + 1) {
		int label = 0;
		int maxnum = 0;
		for (int j = 0; j < 6; j = j + 1) {
			if (resulttable.at<float>(i, j) > maxnum) {
				label = j;
				maxnum = resulttable.at<float>(i, j);
			}
		}
		prediction.at<float>(i, 0) = label;
		prediction.at<float>(i, 1) = testlabel.at<int>(i, 0);
		prediction.at<float>(i, 2) = maxnum * 100 / nbtree;
	}

	int countforest = 0;
	for (int i = 0; i < 60; i = i + 1) {
		if (int(prediction.at<float>(i, 0)) != int(testlabel.at<int>(i, 0))) {
			countforest = countforest + 1;
		}
	}
	std::cout << "The percentage of error with Random Forest with " << nbtree << " trees is "
		<< countforest * 100 / 60
		<< "%\n";

	if (verbose) {
		std::cout << "The predicts, real labels and confidences are: \n"
			<< prediction
			<< "\n";
	}

	// save it in a log file
	ofstream outputtxt;
	outputtxt.open("output_task2.txt");
	outputtxt << "The percentage of error with decision tree with depth 4 is "
		<< counttree * 100 / 60
		<< "%\n"
		<< "The percentage of error with Random Forest with "
		<< nbtree
		<< " trees is "
		<< countforest * 100 / 60
		<< "%\n"
		<< "The predicts, real labels and confidences are: \n"
		<< prediction
		<< "\n";
	outputtxt.close();

	if (verbose) {
		std::cout << "Done!\n";
	}
	waitKey(0);
}


// =-=-=-=-= TASK 3 =-=-=-=-=

// some parameters, defined globally.
// WARNING all folders need to exist, won't create them
string custom_name = "Outputs/pres/"; // to save the images and P/R
string load_path = "Models/aug/10d"; // to load a RF
string save_path = ""; // to save the RF
int numTest = 44;
// // Decision tree parameters
int cvf_3 = 0;
int mc_3 = 2;
int md_3 = 10;
int msc_3 = 2;
int nbtree_3 = 100;
int thresh_3 = 60;
int thresholds[] = { 70 };
int numSampleTrain_3 = 2500; // WARNING 250 if not-augmented
// // Sliding windows parameters
int stride = 10;
// float aspect_ratio = 1.0;
int scales[] = { 50, 75,100,125,150, 175, 200, 250 };// { 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200 };// { 100, 110 };
// // NMS parameters
float intersectT_3 = 0.4;
int NeighborIntersectMin = 0;

// struct of the prediction of the Random Forest over one box
struct boxResult {
	int xmin;
	int ymin;
	int xmax;
	int ymax;
	vector<int> scores; // its ith element stores the number of trees agreeing that this box is i.
};

// gives the likeliest class for a box, ie the majority vote over all trees
int guess(boxResult br) {
	int g = 0;
	for (int i = 1; i < br.scores.size(); i++) {
		if (br.scores[g] < br.scores[i]) {
			g = i;
		}
	}
	return g;
}

// gives the number of votes for the majority vote over all trees for one box
int nbTrees(boxResult br) {
	int g = 0;
	for (int i = 1; i < br.scores.size(); i++) {
		if (br.scores[g] < br.scores[i]) {
			g = i;
		}
	}
	return br.scores[g];
}

// ...
const int area(boxResult br) {
	return (br.xmax - br.xmin)*(br.ymax - br.ymin);
}

// Map a class to a color, background=black
const Scalar& toColor(int guess) {
	if (guess == 0)
		return Scalar(255, 0, 0);
	if (guess == 1)
		return Scalar(0, 255, 0);
	if (guess == 2)
		return Scalar(0, 0, 255);
	return Scalar(0, 0, 0);
}

// gives a box a mix of all three color classes, according to the #votes of each
const Scalar& toColor(vector<int> guess) {
	return Scalar(255 * guess[0] / nbtree_3, 255 * guess[1] / nbtree_3, 255 * guess[2] / nbtree_3);
	return Scalar(0, 0, 0);
}

// Saves in path s a picture adding onto img the bounding boxes contained in list results
void saveRects(Mat img, vector<boxResult> results, string s, bool verbose) {
	for (const boxResult br : results) {
		Point pt1(br.xmin, br.ymin);
		Point pt2(br.xmax, br.ymax);
		rectangle(img, pt1, pt2, toColor(br.scores), (nbTrees(br) - thresh_3 + 1)*5/(nbtree_3 - thresh_3/2));
	}
	if (verbose) {
		imshow("s", img);
	}
	imwrite(s, img);
}

// for testing purposes
void testSaveRects(string s) {
	vector<boxResult> results;
	for (int i = 0; i < 10; i++) {
		boxResult thisBoxResult;
		thisBoxResult.xmin = 10*i;
		thisBoxResult.ymin = 20*i;
		thisBoxResult.xmax = 10*i+100;
		thisBoxResult.ymax = 20*i+100;
		for(int j=0; j<(i%4); j++)
			thisBoxResult.scores.push_back(4);
		thisBoxResult.scores.push_back(10);
		for (int j = (i % 4) + 1; j < 4; j++)
			thisBoxResult.scores.push_back(4);
		results.push_back(thisBoxResult);
	}
	Mat img = imread("../data/task3/test/0022.jpg");
	saveRects(img, results, s, true);
}

//
const int intersect(Point nw1, Point se1, Point nw2, Point se2) {
	/*
	returns the area (in pixels) of the intersection between rectangles 1 and 2,
	where the inputs are these two rectangles' North-West and South-East corners
	*/
	const int lx = min(se1.x, se2.x) - max(nw1.x, nw2.x);
	const int ly = min(se1.y, se2.y) - max(nw1.y, nw2.y);
	if (lx > 0 && ly > 0)
		return lx * ly;
	return 0;
}



// Intersection over Union
const float IOU(Point nw1, Point se1, Point nw2, Point se2) {
	int S1 = (se1.x - nw1.x)*(se1.y - nw1.y);
	int S2 = (se2.x - nw2.x)*(se2.y - nw2.y);
	int Sinter = intersect(nw1, se1, nw2, se2);
	float iou = float(Sinter) / (S1 + S2 - Sinter);
	return iou;
}

vector<boxResult> greedy_NMS_from_nbtree(vector<boxResult> rects, float t) {
	/*
	removes from rects rectangles that are not local maxima with respect to the number of trees that accepted them
	t being the minimal intersection for which we consider rectangles to be neighbors
	*/
	vector<boxResult> res;
	
	for (boxResult rect : rects) {
		// is it maximal enough?
		bool isLocalMax = true;
		const int g = guess(rect);
		for (boxResult rect2 : rects) {

			if (rect2.scores[g] > rect.scores[g] &&
				IOU(Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax), Point(rect2.xmin, rect2.ymin), Point(rect2.xmax, rect2.ymax)) > t) {
				isLocalMax = false;
				break;
			}
		}
		if (isLocalMax) {
			res.push_back(rect);
		}
	}
	return res;

}

const int neighborness(boxResult b1, boxResult b2) {
	// returns 0 if b1 and b2 are not neighbors, and the intersecting area if they are
	//   neighboring condition depends on both rectangles' areas
	//   (if it only depended on the area of intersection, it would advantage large b1,
	//   and if it only also depended on b1's area small b1s would be advantaged)
	const int intersection = intersect(Point(b1.xmin, b1.ymin), Point(b1.xmax, b1.ymax), Point(b2.xmin, b2.ymin), Point(b2.xmax, b2.ymax));
	const int area1 = area(b1);
	const int area2 = area(b2);
	return (3 * intersection > area1 + area2) ? // asks whether A xor B is larger than A /\ B or not
		intersection : 0;
}

vector<boxResult> greedy_NMS_from_neighbors_nbtree(vector<boxResult> rects) {
	/*
	removes from rects rectangles that are not local maxima with respect to
	a weighted (by their IoU) average of their neighbor's number of votes
	for the considered rectangle's majority vote
	*/


	// get parameters to normalize scores over each class
	int norm[3];
	for (int cl = 0; cl < 3; cl++) {
		norm[cl] = 0;
		for (boxResult rect : rects) {
			norm[cl] += rect.scores[cl];
		}
		norm[cl] /= rects.size();
	}

	// measure the confidence of rect's prediction
	vector<int> confidence;
	for (boxResult rect : rects) {
		int rclass = guess(rect); // the class of rect
		int rconf = 0; // the confidence of that class
		for (boxResult rect2 : rects) {
			rconf += neighborness(rect, rect2) * (rect2.scores[rclass] - norm[rclass]);
		}
		confidence.push_back(rconf / area(rect));
	}

	// NMS proper
	vector<boxResult> res;
	for (int i = 0; i < rects.size(); i++) {
		boxResult rect = rects[i];
		int rconf = confidence[i];
		bool isLocalMax = true;
		const int neighCAT = area(rect) / 4; // minimal intersection area to call someone a neighbor
		for (int j = 0; j < rects.size(); j++) {
			boxResult rect2 = rects[j];
			if (confidence[j] > confidence[i] &&
				intersect(Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax), Point(rect2.xmin, rect2.ymin), Point(rect2.xmax, rect2.ymax)) > neighCAT) {
				isLocalMax = false;
				break;
			}
		}
		if (isLocalMax) {
			res.push_back(rect);
		}
	}

	return res;
}

// Abandoned strategy
float certTry1(const int desired, const Point nw, const Point se, vector<boxResult> rects) {
	/*
	returns the ratio between the number of boxes having the desired class, and the total,
	among rectangles intersecting (nw,se) weighted by their intersection AND their nbTrees
	*/
	int tottotal = 0;
	int destotal = 0;
	for (boxResult rect : rects) {
		if (guess(rect) != 3) {
			const int in = intersect(nw, se, Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax));
			tottotal += in * (nbTrees(rect) - thresh_3 + 1);
			if (guess(rect) == desired)
				destotal += in;
		}
	}
	return float(destotal) / float(tottotal);
}

// Abandoned strategy
vector<boxResult> cullOutliersFromIntersect(vector<boxResult> rects, float t) {
	/*
	removes from rects rectangles that disagree with "too many" rectangles that intersect with them, threshold fixed by t
	WARNING call it only on a list of same(-enough)-scaled rectangles, to avoid dominance of large objects i.a.
	WARNING not very efficient, if at all: use NMS instead...
	*/
	vector<boxResult> res;
	for (const boxResult rect : rects) {
		if (certTry1(guess(rect), Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax), rects) > t)
			res.push_back(rect);
	}
	return res;
	
}

// gives path to image i if the folders 00,01,02,03 were concatenated.
string dirty_path_to_image(int i) {
	const int nb[] = { 530,810,510,2900 }; // WARNING nb_samples of each class hard-coded
	for (int cl = 0; cl < sizeof(nb); cl++) {
		if (i < nb[cl]) {
			string b1 = to_string(i);
			string b2 = string(4 - b1.length(), '0') + b1;
			return to_string(cl)+"/"+b2+".jpg";
		}
		i -= nb[cl];
	}
	std::cout << "[ERROR] Bounds given in nb_samples exceeded !" << endl;

}

// train one tree on a dynamically generated subset of data (avoids saving the whole data during execution)
Ptr<ml::DTrees> train_tree(string folderName, HOGDescriptor *hog, bool verbose, int ii) {
	const int numTrain = 10 * (53 + 81 + 51 + 290); //WARNING hardcoded
	
	// L oad the data
	vector<int> generator(numTrain);
	Mat subtraindata;
	Mat subtrainlabel;
	for (int i = 0; i < numTrain; i++) {
		generator[i] = i;
	}
	random_shuffle(generator.begin(), generator.end());
	for (int i = 0; i < numSampleTrain_3; i++) {
		string dpti = dirty_path_to_image(generator[i]);
		loadImg(subtraindata, subtrainlabel, hog,
			folderName + "/train/0" + dpti, dpti.at(0) - '0');
	}
	subtraindata.convertTo(subtraindata, CV_32F);
	subtraindata = subtraindata.reshape(1, numSampleTrain_3);

	// C reate the tree
	Ptr<ml::DTrees> dtree;
	dtree = createTree(cvf_3, mc_3, md_3, msc_3);
	Ptr<ml::TrainData> tData = ml::TrainData::create(subtraindata, 
		ml::ROW_SAMPLE, subtrainlabel);
	dtree->train(tData);
	if (verbose)
		std::cout << " Tree # " << ii + 1 << " trained,"
		<< " train error: " << dtree->calcError(tData, false, noArray())
		<< "     \r" << flush;
	return dtree;
}

// main pipeline for task 3
void task3(const cv::String folderName, const bool verbose, const bool interactive) {
	//
	// train
	//

	//variable initialization
	
	if (interactive) {
		int yesno = inputNumber("Want to compute a new RF? (Currently loads RF from [ "+load_path+" ])");//("Want to change Decision Forest parameters? (yes=1)  ");
		if (yesno == 1) {
			load_path = "";
			//cvf_3 = inputNumber("Change CVFolds (default " + to_string(cvf_3) + "): ");
			//mc_3 = inputNumber("Change MaxCategories (default " + to_string(mc_3) + "): ");
			//md_3 = inputNumber("Change MaxDepth (default " + to_string(md_3) + "): ");
			//msc_3 = inputNumber("Change MinSampleCount (default " + to_string(msc_3) + "): ");
			//numTest = inputNumber("Change number of test images (default " + to_string(numTest) + "): ");
			//cout << "Add a custom prefix to the output files: " << custom_name; getline(cin, custom_name);
			//cout << "Pre-load already saved random-forest? "<< load_path; getline(cin, load_path);
			//nbtree_3 = inputNumber("Change nbTree (default " + to_string(nbtree_3) + "): ");
			//thresh_3 = inputNumber("Change ConsensusThreshold (default " + to_string(thresh_3) + "): ");
			//intersectT_3 = 0.01f * inputNumber("Change ConsensusThreshold (default " + to_string(int(100*intersectT_3)) + "): ");
			//std::cout << "Ok.\n";
		}
	}

	// P arameters
	const Size winSize(128, 104);
	const Size blockSize(32, 26);
	const Size blockStride(16, 13);
	const Size cellSize(16, 13);

	//train random forest
	vector<Ptr<ml::DTrees>> dForest;
	HOGDescriptor *hog = new HOGDescriptor(winSize, blockSize, blockStride, cellSize, 9);
	for (int i = 0; i < nbtree_3; i++) {
		Ptr<ml::DTrees> dtree;
		if (load_path.size() == 0) { // then we need to train a tree
			dtree = train_tree(folderName, hog, verbose, i);
			dtree->save(save_path + to_string(i) + ".txt");
		}
		else {
			dtree = ml::DTrees::load(load_path + to_string(i) + ".txt");
		}
		dForest.push_back(dtree);
	}
	if (verbose) {
		std::cout << "\nForest loaded/trained." << endl;
	}

	ofstream biglogpr;
	biglogpr.open(custom_name + "whole_pr.txt");
	biglogpr << "Threshold Precision Recall\n";


	for (int th : thresholds) {
		std::cout << "[BIGLOOP] threshold=" << th << endl;
		vector<boxResult> results;
		thresh_3 = th;
		ofstream logpr;
		logpr.open(custom_name + to_string(th) + "_pr.txt");
		logpr << "Precision Recall\n";
		int correctpredict = 0; //number of correct predicted boxes, nominator of precision and recall
		int predictedbox = 0;  //number of predicted boxes, denominator of precision
		int gtbox = 0;  //number of ground true boxes, denominator of recall

		// for all images in test folder
		for (int i = 0; i<numTest; i++) {
			results = vector<boxResult>();
			string ii = (i < 10) ? "0" + to_string(i) : to_string(i);
			Mat img;
			Mat img1;
			string imgpath = folderName + "/test/00" + ii + ".jpg";
			img1 = imread(imgpath);
			cvtColor(img1, img, COLOR_RGB2GRAY);

			//
			// a) generate bounding boxes (Sliding Windows)
			//


			// create all bounding boxes

			for (const int &scale : scales) {
				if (verbose) {
					std::cout << "[Image " << ii << "]  Scale: " << scale << "\r" << flush;
				}
				for (int x = 0; x + scale < img.cols; x += stride) {
					for (int y = 0; y + scale < img.rows; y += stride) {
						Mat box = img(Rect(x, y, scale, scale));
						resize(box, box, Size(128, 104));
						vector<float> descriptors;
						hog->compute(box, descriptors, Size(2, 2), Size(0, 0));
						Mat boxHogData;
						boxHogData.push_back(descriptors);
						boxHogData = boxHogData.reshape(1, 1);
						boxHogData.convertTo(boxHogData, CV_32F);

						//
						// b) classify their contents
						//

						vector<double> resulttable(nbtree_3);
						Mat output;
						for (Ptr<ml::DTrees> dtree : dForest) {
							int response = int(dtree->predict(boxHogData, output, ml::StatModel::Flags::RAW_OUTPUT));
							resulttable[response] ++;// No hope here, output is 1x1 matrix with same result as response, even with RAW_OUTPUT u.u

						}
						int label = 0;
						float maxnum = 0;

						for (int j = 0; j < 4; j++) {
							if (resulttable.at(j) > maxnum) {
								label = j;
								maxnum = resulttable.at(j);
							}
							else if (resulttable.at(j) == maxnum) {
								label = 3; // if doubt, say it's background
							}
						}
						// store results if not majoritarily background
						if (label != 3 && maxnum >= thresh_3) {
							boxResult thisBoxResult;
							thisBoxResult.xmin = x;
							thisBoxResult.ymin = y;
							thisBoxResult.xmax = x + scale;
							thisBoxResult.ymax = y + scale;
							for (int i = 0; i < 4; i++) {
								thisBoxResult.scores.push_back(resulttable[i]);
							}
							results.push_back(thisBoxResult);
						}
					}
				}
			}

			// display results
			Mat img2 = img1.clone();
			saveRects(img1, results, custom_name + "before_nms_" + ii + ".jpg", false);

			//
			// c) non-maximum suppression
			//

			if (results.size() != 0) {
				results = greedy_NMS_from_nbtree(results, 0.5);
				results = greedy_NMS_from_neighbors_nbtree(results);
				saveRects(img2, results, custom_name + "after_nms_" + ii + ".jpg", false);
				
				//
				// d) evaluate result
				//
				string gtpath = folderName + "/gt/00" + ii + ".gt.txt";

				ifstream gtfile;
				gtfile.open(gtpath);
				int gt[15] = { 0 };
				for (int i = 0; i < 15; i++) {
					gtfile >> gt[i];
				}
				gtfile.close();
				predictedbox += results.size();
				gtbox += 3;
				bool crps[] = { false,false,false }; // to avoid recall exceeding 1
				for (boxResult res : results) {
					int g = guess(res);
					if (IOU(Point(gt[1 + 5 * guess(res)], gt[2 + 5 * g]),
							Point(gt[3 + 5 * guess(res)], gt[4 + 5 * g]),
							Point(res.xmin, res.ymin),
							Point(res.xmax, res.ymax))
					> 0.5) {
						crps[g] = true;
					}
				}
				int crp = crps[0] + crps[1] + crps[2];
				correctpredict += crp;
				if (verbose)
					std::cout << "\t\t\t\t" << "Recall: " << float(crp) / 3 << "\r" << flush <<
					"[Image " << ii << "] Precision: " << float(crp) / results.size() << endl;
				logpr << float(crp) / results.size() << " " << float(crp) / 3 << endl;
			}
			else {
				gtbox += 3;
				if (verbose)
					std::cout << "\t\t\t\t" << "Recall: " << 0 << "\r" << flush <<
					"[Image " << ii << "] Precision: N/A" << endl;
				logpr << "N/A 0" << endl;
				
			}
			
			
		}
		std::cout << "\t\t\t\t" << "Recall: " << float(correctpredict) / (gtbox) << "\r" << flush <<
			"[Overall]  Precision: " << float(correctpredict) / (predictedbox) << endl;
		logpr << endl << float(correctpredict) / (predictedbox) << " " << float(correctpredict) / (gtbox) << endl;
		logpr.close();
		biglogpr << th << " " << float(correctpredict) / (predictedbox) << " " << float(correctpredict) / (gtbox) << endl;
	}
	biglogpr.close();
	
}

// =-=-=-=-= MAIN =-=-=-=-=

void help() {
	std::cout << "\n=-= EXERCISE 2 TDCV =-=\n"
		"\nThis program computes tasks related to exercise 2 of the WS2017-18 TDCV course at TUM\n"
		"(about random forests of HOG)\n"
		"Possible modes are:\n"
			"\t0- Data augmentation\n"
            "\t1- Task 1, compute and display HOG of an image\n"
            "\t2- Task 2, train and test a random forest to classify images\n"
            "\t3- Task 3, get P/R results after training a random forest to detect features in images\n"
		"\nUsage and examples\n"
		"  ex2.exe [-hvi][--help][--verbose][--interactive][mode][filename]\n"
		"  x64\\Release\\ex2.exe 1 data/task1/obj1000.jpg\n"
		"  x64\\Release\\ex2.exe 2 data/task2\n"
		"  x64\\Release\\ex2.exe -v -i 3 data/task3/aug\n"
		"  x64\\Release\\ex2.exe -v 0 data/task3/\n"
		"\nDetails\n"
		"\tAuthors Lapostre Thomas & Zhu Yue" << endl;
}


int main(int argc, char** argv)
{
	srand(time(NULL));
	int task;
	string inputName;
	bool verbose = false;
	bool interactive = false;
	cv::CommandLineParser parser(argc, argv,
		"{help h||}"
		"{verbose v||}"
		"{interactive i||}"
		"{@task_number||}{@filename||}"
	);
	task = parser.get<int>("@task_number");

	inputName = parser.get<string>("@filename");

	if (parser.has("help"))
	{
		help();
		return 0;
	} else 	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	if (parser.has("verbose")) {
		verbose = true;
		std::cout << "Verbose mode is on, yay!\n\n";
	}
	if (parser.has("interactive")) {
		interactive = true;
		std::cout << "Interactive mode is on, yay!\n\n";
	}

	bool redo = true;
	while (redo) {
		if (task == 1) {
			return task1(inputName);
		}
		else if (task == 2) {
			task2(inputName, verbose, interactive);
		}
		else if (task == 3) {
			task3(inputName, verbose, interactive);
			//testSaveRects("test.jpg");
		}
		else if (task == 0) {
			data_aug(inputName, verbose);
		}
		else {
			std::cout << "Task number not existent or not implemented yet" << endl;
			return -1;
		}
		redo = false;
		if (interactive) {
			int q = inputNumber("Want to try again? (yes=1) ");
			redo = (q == 1);
		}
	}
	

	waitKey(0);
	return 0;
}

