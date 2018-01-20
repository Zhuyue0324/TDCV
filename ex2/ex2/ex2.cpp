// ex2.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp> 
#include <opencv2/objdetect.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "data_augmentation.h"
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <stdio.h>
#include <stddef.h>

using namespace cv;
using namespace std;


/*using namespace boost::filesystem; // => will install it later, tinkering should do for now

//#include <boost/filesystem.hpp>
struct recursive_directory_range
{
	typedef recursive_directory_iterator iterator;
	recursive_directory_range(path p) : p_(p) {}

	iterator begin() { return recursive_directory_iterator(p_); }
	iterator end() { return recursive_directory_iterator(); }

	path p_;
};*/

int inputNumber(string msg) {
	int myNumber = 0;
	string input = "";
	while (true) {
		cout << msg;
		getline(cin, input);

		// This code converts from string to number safely.
		stringstream myStream(input);
		if (myStream >> myNumber)
			break;
		cout << "Invalid number, please try again" << endl;
	}
	return myNumber;
}

// =-=-=-=-= TASK 1 =-=-=-=-=

void visualizeHOG(cv::Mat img, std::vector<float> &feats, cv::HOGDescriptor hog_detector, int scale_factor = 3);
/*
* img - the image used for computing HOG descriptors. **Attention here the size of the image should be the same as the window size of your cv::HOGDescriptor instance **
* feats - the hog descriptors you get after calling cv::HOGDescriptor::compute
* hog_detector - the instance of cv::HOGDescriptor you used
* scale_factor - scale the image *scale_factor* times larger for better visualization
*/


void visualizeHOG(cv::Mat img, std::vector<float> &feats, cv::HOGDescriptor hog_detector, int scale_factor) {

	cv::Mat visual_image;
	resize(img, visual_image, cv::Size(img.cols * scale_factor, img.rows * scale_factor));

	int n_bins = hog_detector.nbins;
	float rad_per_bin = 3.14 / (float)n_bins;
	cv::Size win_size = hog_detector.winSize;
	cv::Size cell_size = hog_detector.cellSize;
	cv::Size block_size = hog_detector.blockSize;
	cv::Size block_stride = hog_detector.blockStride;

	// prepare data structure: 9 orientation / gradient strenghts for each cell
	int cells_in_x_dir = win_size.width / cell_size.width;
	int cells_in_y_dir = win_size.height / cell_size.height;
	int n_cells = cells_in_x_dir * cells_in_y_dir;
	int cells_per_block = (block_size.width / cell_size.width) * (block_size.height / cell_size.height);

	int blocks_in_x_dir = (win_size.width - block_size.width) / block_stride.width + 1;
	int blocks_in_y_dir = (win_size.height - block_size.height) / block_stride.height + 1;
	int n_blocks = blocks_in_x_dir * blocks_in_y_dir;

	float ***gradientStrengths = new float **[cells_in_y_dir];
	int **cellUpdateCounter = new int *[cells_in_y_dir];
	for (int y = 0; y < cells_in_y_dir; y++) {
		gradientStrengths[y] = new float *[cells_in_x_dir];
		cellUpdateCounter[y] = new int[cells_in_x_dir];
		for (int x = 0; x < cells_in_x_dir; x++) {
			gradientStrengths[y][x] = new float[n_bins];
			cellUpdateCounter[y][x] = 0;

			for (int bin = 0; bin < n_bins; bin++)
				gradientStrengths[y][x][bin] = 0.0;
		}
	}


	// compute gradient strengths per cell
	int descriptorDataIdx = 0;


	for (int block_x = 0; block_x < blocks_in_x_dir; block_x++) {
		for (int block_y = 0; block_y < blocks_in_y_dir; block_y++) {
			int cell_start_x = block_x * block_stride.width / cell_size.width;
			int cell_start_y = block_y * block_stride.height / cell_size.height;

			for (int cell_id_x = cell_start_x;
				cell_id_x < cell_start_x + block_size.width / cell_size.width; cell_id_x++)
				for (int cell_id_y = cell_start_y;
					cell_id_y < cell_start_y + block_size.height / cell_size.height; cell_id_y++) {

				for (int bin = 0; bin < n_bins; bin++) {
					float val = feats.at(descriptorDataIdx++);
					gradientStrengths[cell_id_y][cell_id_x][bin] += val;
				}
				cellUpdateCounter[cell_id_y][cell_id_x]++;
			}
		}
	}


	// compute average gradient strengths
	for (int celly = 0; celly < cells_in_y_dir; celly++) {
		for (int cellx = 0; cellx < cells_in_x_dir; cellx++) {

			float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

			// compute average gradient strenghts for each gradient bin direction
			for (int bin = 0; bin < n_bins; bin++) {
				gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
			}
		}
	}


	for (int celly = 0; celly < cells_in_y_dir; celly++) {
		for (int cellx = 0; cellx < cells_in_x_dir; cellx++) {
			int drawX = cellx * cell_size.width;
			int drawY = celly * cell_size.height;

			int mx = drawX + cell_size.width / 2;
			int my = drawY + cell_size.height / 2;

			rectangle(visual_image,
				cv::Point(drawX * scale_factor, drawY * scale_factor),
				cv::Point((drawX + cell_size.width) * scale_factor,
				(drawY + cell_size.height) * scale_factor),
				CV_RGB(100, 100, 100),
				1);

			for (int bin = 0; bin < n_bins; bin++) {
				float currentGradStrength = gradientStrengths[celly][cellx][bin];

				if (currentGradStrength == 0)
					continue;

				float currRad = bin * rad_per_bin + rad_per_bin / 2;

				float dirVecX = cos(currRad);
				float dirVecY = sin(currRad);
				float maxVecLen = cell_size.width / 2;
				float scale = scale_factor / 5.0; // just a visual_imagealization scale,

												  // compute line coordinates
				float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
				float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
				float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
				float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

				// draw gradient visual_imagealization
				line(visual_image,
					cv::Point(x1 * scale_factor, y1 * scale_factor),
					cv::Point(x2 * scale_factor, y2 * scale_factor),
					CV_RGB(0, 0, 255),
					1);

			}

		}
	}


	for (int y = 0; y < cells_in_y_dir; y++) {
		for (int x = 0; x < cells_in_x_dir; x++) {
			delete[] gradientStrengths[y][x];
		}
		delete[] gradientStrengths[y];
		delete[] cellUpdateCounter[y];
	}
	delete[] gradientStrengths;
	delete[] cellUpdateCounter;
	cv::imshow("HOG vis", visual_image);
	cv::waitKey(-1);
	cv::imwrite("hog_vis.jpg", visual_image);

}

int task1(const cv::String &filename)
{
	Mat img = imread(filename);
	if (img.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
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

void loadImgs(Mat &data, Mat &label, HOGDescriptor *hog, const cv::String &folderName, const int beginIdx, const int endIdx, const int lbl) {
	string ii;
	string imgpath;
	for (int i = beginIdx; i < endIdx; i++) {
		//TODO find inbuilt function to do it efficiently
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
			cout << "Could not read image " << imgpath << endl;
		}
	}
}

void loadImgs(Mat &data, Mat &label, HOGDescriptor *hog, const cv::String &folderName, const int lbl) {
	string ii;
	string imgpath;
	for (int i = 0;; i++) {
		//TODO find inbuilt function to do it efficiently
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
			cout << "Stopped at image " << imgpath << endl;
			break;
		}
	}
}

void loadTrain(Mat &traindata, Mat &trainlabel, HOGDescriptor *hog, const cv::String &folderName) {
	loadImgs(traindata, trainlabel, hog, folderName + "/00/0", 0, 49, 0);
	loadImgs(traindata, trainlabel, hog, folderName + "/01/0", 0, 67, 1);
	loadImgs(traindata, trainlabel, hog, folderName + "/02/0", 0, 42, 2);
	loadImgs(traindata, trainlabel, hog, folderName + "/03/0", 0, 53, 3);
	loadImgs(traindata, trainlabel, hog, folderName + "/04/0", 0, 67, 4);
	loadImgs(traindata, trainlabel, hog, folderName + "/05/0", 0, 110, 5);
}

void loadTest(Mat &testdata, Mat &testlabel, HOGDescriptor *hog, const cv::String &folderName) {
	loadImgs(testdata, testlabel, hog, folderName + "/00/0", 49, 59, 0);
	loadImgs(testdata, testlabel, hog, folderName + "/01/0", 67, 77, 1);
	loadImgs(testdata, testlabel, hog, folderName + "/02/0", 42, 52, 2);
	loadImgs(testdata, testlabel, hog, folderName + "/03/0", 53, 63, 3);
	loadImgs(testdata, testlabel, hog, folderName + "/04/0", 67, 77, 4);
	loadImgs(testdata, testlabel, hog, folderName + "/05/0", 110, 120, 5);
}

Ptr<ml::DTrees> createTree(int cvf, int mc, int md, int msc) {
	Ptr<ml::DTrees> dtree = ml::DTrees::create();
	dtree->setCVFolds(cvf);
	dtree->setMaxCategories(mc);
	dtree->setMaxDepth(md);
	dtree->setMinSampleCount(msc);
	return(dtree);
}

void task2(const cv::String folderName, const bool verbose, const bool interactive) {

	//variable initialization
	Mat traindata;
	Mat testdata;
	Mat trainlabel;
	Mat testlabel;

	Size winSize(128, 104);
	Size blockSize(32, 26);
	Size blockStride(16, 13);
	Size cellSize(16, 13);
	HOGDescriptor *hog = new HOGDescriptor(winSize, blockSize, blockStride, cellSize, 9);

	//prepare the train data, train label, test data and test label and reform them into proper form 
	loadTrain(traindata, trainlabel, hog, folderName + "/train"); //388
	loadTest(testdata, testlabel, hog, folderName + "/test");  //60

	traindata.convertTo(traindata, CV_32F);
	traindata = traindata.reshape(1, 388);
	testdata.convertTo(testdata, CV_32F);
	testdata = testdata.reshape(1, 60);

	if (verbose) {
		cout << "Data loaded\n";
	}

	//Decision Tree part
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
			cout << "Ok.\n";
		}
	}
	Ptr<ml::DTrees> dtree = createTree(cvf, mc, md, msc);
	Ptr<ml::TrainData> tData = ml::TrainData::create(traindata, ml::ROW_SAMPLE, trainlabel);
	dtree->train(tData);
	Mat output;
	float response = dtree->predict(testdata, output);
	int counttree = 0;
	for (int i = 0; i < 60; i = i + 1) {
		if (int(output.at<float>(i, 0)) != int(testlabel.at<int>(i, 0))) {
			counttree = counttree + 1;
		}
	}
	cout << "The percentage of error with decision tree with depth 4 is "
		<< counttree * 100 / 60
		<< "%\n";

	//Forest part
	int nbtree = 8;
	if (interactive) {
		int yesno = inputNumber("Want to change Decision Forest parameters? (yes=1)  ");
		if (yesno == 1) {
			nbtree = inputNumber("Change nbTree (default " + to_string(nbtree) + "): ");
			cvf = inputNumber("Change CVFolds (default " + to_string(cvf) + "): ");
			mc = inputNumber("Change MaxCategories (default " + to_string(mc) + "): ");
			md = inputNumber("Change MaxDepth (default " + to_string(md) + "): ");
			msc = inputNumber("Change MinSampleCount (default " + to_string(msc) + "): ");
			cout << "Ok.\n";
		}
	}
	/*Mat output;*/
	Mat resulttable = Mat::zeros(60, 6, CV_32F);
	
	for (int i = 0; i < nbtree; i = i + 1) {
		Mat subtraindata;
		Mat subtrainlabel;
		//generate a random subset
		vector<int> generator(388);

		for (int i = 0; i < 388; i = i + 1) {
			generator[i] = i;
		}
		random_shuffle(generator.begin(), generator.end());
		for (int i = 0; i < 300; i = i + 1) {
			subtraindata.push_back(traindata.row(generator[i]));
			subtrainlabel.push_back(trainlabel.row(generator[i]));
		}

		Ptr<ml::DTrees> dtree = createTree(cvf, mc, md, msc);
		Ptr<ml::TrainData> tData = ml::TrainData::create(subtraindata, ml::ROW_SAMPLE, subtrainlabel);
		dtree->train(tData);
		float response = dtree->predict(testdata, output);
		for (int j = 0; j < 60; j = j + 1) {
			resulttable.at<float>(j, int(output.at<float>(j, 0))) = resulttable.at<float>(j, int(output.at<float>(j, 0))) + 1;
		}
	}
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
	cout << "The percentage of error with Random Forest with " << nbtree << " trees is "
		<< countforest * 100 / 60
		<< "%\n";

	if (verbose) {
		cout << "The predicts, real labels and confidences are: \n"
			<< prediction
			<< "\n";
	}

	ofstream outputtxt;
	outputtxt.open("output_task2.txt");// TODO give as arguments (eventually pre-set)
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
		cout << "Done!\n";
	}
	//wait
	//system("pause");
	waitKey(0);
}


// =-=-=-=-= TASK 3 =-=-=-=-=

// some parameters, defined globally. _3 is added for clarity
string custom_name = "Outputs/testneigh/";
string load_path = "Models/shallow/tree_";
// // Decision tree parameters
int cvf_3 = 0;
int mc_3 = 2;
int md_3 = 3;
int msc_3 = 2;
int nbtree_3 = 100;
int thresh_3 = 60;
int numSampleTrain_3 = 250;
// // Sliding windows parameters
int stride = 10;
// float aspect_ratio = 1.0;
int scales[] = { 75,100,125,150, 175 };// { 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200 };// { 100, 110 };
// // NMS parameters
float intersectT_3 = 0.4;
int NeighborIntersectMin = 0;


struct boxResult {
	int xmin;
	int ymin;
	int xmax;
	int ymax;
	vector<int> scores; // its ith element stores the number of trees agreeing that this box is i.
};

int guess(boxResult br) {
	int g = 0;
	for (int i = 1; i < br.scores.size(); i++) {
		if (br.scores[g] < br.scores[i]) {
			g = i;
		}
	}
	return g;
}

int nbTrees(boxResult br) {
	int g = 0;
	for (int i = 1; i < br.scores.size(); i++) {
		if (br.scores[g] < br.scores[i]) {
			g = i;
		}
	}
	return br.scores[g];
}

const int area(boxResult br) {
	return (br.xmax - br.xmin)*(br.ymax - br.ymin);
}

const bool isoloq(boxResult br1, boxResult br2) {
	// says whether br1 and br2 correspond to the same rectangle
	return br1.xmin == br2.xmin && br1.xmax == br2.xmax && br1.ymin == br2.ymin && br1.ymax == br2.ymax;
}

const Scalar& toColor(int guess) {
	/*
	Maps a class to a color
	*/
	if (guess == 1)
		return Scalar(0, 0, 255);
	if (guess == 2)
		return Scalar(0, 255, 0);
	if (guess == 0)
		return Scalar(255, 0, 0);
	return Scalar(0, 0, 0);
}

const Scalar& toColor(vector<int> guess) {
	/*
	Maps a class to a color
	*/
	return Scalar(255 * guess[0] / nbtree_3, 255 * guess[1] / nbtree_3, 255 * guess[2] / nbtree_3);
	return Scalar(0, 0, 0);
}

void saveRects(Mat img, vector<boxResult> results, string s, bool verbose) {
	/*
	Saves in path s a picture adding onto img the bounding boxes contained in list results
	*/
	for (const boxResult br : results) {
		Point pt1(br.xmin, br.ymin);
		Point pt2(br.xmax, br.ymax);
		rectangle(img, pt1, pt2, toColor(br.scores), (nbTrees(br) - thresh_3 + 1)*5/(nbtree_3 - thresh_3/2));
	}
	if (verbose) {
		imshow("s", img);
		//cv::waitKey(-1);
	}
	imwrite(s, img);
}

void testDisplayRects(string s) {
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
	return float(destotal)/float(tottotal);
}

vector<boxResult> greedy_NMS_from_nbtree(vector<boxResult> rects, int t) {
	/*
	removes from rects rectangles that are not local maxima with respect to the number of trees that accepted them
	t being the minimal intersection for which we consider rectangles to be neighbors
	*/
	vector<boxResult> res;
	
	for (boxResult rect : rects) {
		// is it maximal enough?
		bool isLocalMax = true;
		for (boxResult rect2 : rects) {
			if (nbTrees(rect2) > nbTrees(rect) &&
				intersect(Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax), Point(rect2.xmin, rect2.ymin), Point(rect2.xmax, rect2.ymax)) > t) {
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

vector<boxResult> greedy_NMS_from_neighbors_nbtree(vector<boxResult> rects) {
	// we already know that all rectangles are above confidence threshold

	vector<boxResult> res;
	vector<int> confidence;
	for (boxResult rect : rects) {
		// measure its maximality metric
		int rclass = guess(rect); // the class of rect
		int rconf = 0; // the confidence of that class
		const int neighCAT = (rect.ymax - rect.ymin)*(rect.xmax - rect.xmin) / 4; // minimal intersection area to call someone a neighbor

		//int nbneigh = 0; // number of neighbors considered, for normalization purposes

		for (boxResult rect2 : rects) {
			
			int intersection = intersect(Point(rect.xmin, rect.ymin), Point(rect.xmax, rect.ymax), Point(rect2.xmin, rect2.ymin), Point(rect2.xmax, rect2.ymax));
			if (intersection > neighCAT) {
				rconf += intersection * (rect2.scores[rclass] - thresh_3);
				//nbneigh+= intersection;
			}
		}
		confidence.push_back(rconf / area(rect)); // bias towards small one from majority class
		//confidence.push_back(rconf); // bias towards large ones
	}
	for (int i = 0; i < rects.size(); i++) {
		boxResult rect = rects[i];
		int rconf = confidence[i];
		bool isLocalMax = true;
		const int neighCAT = (rect.ymax - rect.ymin)*(rect.xmax - rect.xmin) / 4; // minimal intersection area to call someone a neighbor
		for (int j = 0; j < rects.size(); j++) {
			boxResult rect2 = rects[j];
			if (confidence[j] > confidence[i] && // > needed to avoid self-elimination
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

void task3(const cv::String folderName, const bool verbose, const bool interactive) {
	//
	// train
	//

	//variable initialization
	Mat traindata;
	Mat trainlabel;

	Size winSize(128, 104);
	Size blockSize(32, 26);
	Size blockStride(16, 13);
	Size cellSize(16, 13);
	HOGDescriptor *hog = new HOGDescriptor(winSize, blockSize, blockStride, cellSize, 9);

	if (interactive) {
		int yesno = inputNumber("Want to change Decision Forest parameters? (yes=1)  ");
		if (yesno == 1) {
			//cvf_3 = inputNumber("Change CVFolds (default " + to_string(cvf_3) + "): ");
			//mc_3 = inputNumber("Change MaxCategories (default " + to_string(mc_3) + "): ");
			//md_3 = inputNumber("Change MaxDepth (default " + to_string(md_3) + "): ");
			//msc_3 = inputNumber("Change MinSampleCount (default " + to_string(msc_3) + "): ");
			cout << "Add a custom prefix to the output files: "; getline(cin, custom_name);
			cout << "Pre-load already saved random-forest?"; getline(cin, load_path);
			nbtree_3 = inputNumber("Change nbTree (default " + to_string(nbtree_3) + "): ");
			thresh_3 = inputNumber("Change ConsensusThreshold (default " + to_string(thresh_3) + "): ");
			//intersectT_3 = 0.01f * inputNumber("Change ConsensusThreshold (default " + to_string(int(100*intersectT_3)) + "): ");
			cout << "Ok.\n";
		}
	}

	const int numTrain = 53 + 81 + 51 + 290;
	const int numTest = 44;

	if (load_path.size() == 0) { // else we are loading an already computed forest. //TODO untangle ifs and fors, some consts don't need to be defined everytime
		//prepare the train data, train label, test data and test label and reform them into proper form 
		loadImgs(traindata, trainlabel, hog, folderName + "/train/00/0", 0);
		loadImgs(traindata, trainlabel, hog, folderName + "/train/01/0", 1);
		loadImgs(traindata, trainlabel, hog, folderName + "/train/02/0", 2);
		loadImgs(traindata, trainlabel, hog, folderName + "/train/03/0", 3);

		traindata.convertTo(traindata, CV_32F);
		traindata = traindata.reshape(1, numTrain);

		if (verbose) {
			cout << "Data loaded\n";
		}

	}

	//train random forest

	Mat subtraindata;
	Mat subtrainlabel;
	//Ptr<ml::DTrees> dForest [8];
	vector<Ptr<ml::DTrees>> dForest;
	// vector<float> treeConfidence; // supposed to store  < 100 - TrainError >  of each tree

		
	for (int i = 0; i < nbtree_3; i ++) {
		Ptr<ml::DTrees> dtree;
		if (load_path.size() == 0) {
			//generate a random subset
			vector<int> generator(numTrain);

			for (int i = 0; i < numTrain; i++) {
				generator[i] = i;
			}
			random_shuffle(generator.begin(), generator.end());
			for (int i = 0; i < numSampleTrain_3; i++) {
				subtraindata.push_back(traindata.row(generator[i]));
				subtrainlabel.push_back(trainlabel.row(generator[i]));
			}

			dtree = createTree(cvf_3, mc_3, md_3, msc_3);
			Ptr<ml::TrainData> tData = ml::TrainData::create(subtraindata, ml::ROW_SAMPLE, subtrainlabel);
			dtree->train(tData);
			if(verbose)
				cout << " Tree # " << i+1 << " trained, train error: " << dtree->calcError(tData, false, noArray()) << endl;
			//treeConfidence.push_back();
			dtree->save("Models/tree_" + to_string(i) + ".txt");
		}
		else {
			//cout << load_path << i << ".txt";
			dtree = ml::DTrees::load(load_path + to_string(i) + ".txt");
		}
		dForest.push_back(dtree);
	}

	if (verbose) {
		cout << "Forest trained" << endl;
	}

	vector<boxResult> results;

	// for all images in test folder
	for (int i = 0; i<numTest; i++) {// (int i = 0; i<1; i++){ //
		results = vector<boxResult>(); //Thomas is stupid as fuck
		string ii = (i < 10) ? "0"+to_string(i) : to_string(i);
		Mat img;
		Mat img1;
		string imgpath = folderName + "/test/00" + ii + ".jpg";
		img1 = imread(imgpath);
		cvtColor(img1, img, COLOR_RGB2GRAY);
		
		if (verbose) {
			cout << " Image: " << ii << endl;
		}

		//
		// a) generate bounding boxes (Sliding Windows)
		//

		
		// create all bounding boxes

		for (const int &scale : scales) {
			if (verbose) {
				cout << "  Scale: " << scale << endl;
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

					//TODO use random forest to get class
					vector<double> resulttable (nbtree_3);
					Mat output;
					for (Ptr<ml::DTrees> dtree : dForest) {
						int response = int(dtree->predict(boxHogData, output, ml::StatModel::Flags::RAW_OUTPUT));
						resulttable[response] ++;// No hope here, output is 1x1 matrix with same result as response, even with RAW_OUTPUT u.u
						
					}
					int label = 0;
					float maxnum = 0;
					
					for (int j = 0; j < 4; j ++) {
						if (resulttable.at(j) > maxnum) {
							label = j;
							maxnum = resulttable.at(j);
						}
						else if (resulttable.at(j) == maxnum) {
							label = 3; // if doubt, say it's background => shouldn't happen with floats but anyway ...
						}
					}
					// store results if not background
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
		saveRects(img1, results, custom_name+"before_nms_"+ii+".jpg", false);

		//
		// c) non-maximum suppression
		//

		//results = cullOutliersFromIntersect(results, intersectT_3);
		results = greedy_NMS_from_neighbors_nbtree(results);
		saveRects(img2, results, custom_name + "after_nms_" + ii + ".jpg", false);

		//TODO (we still get too many ...)

		//
		// d) evaluate result
		//

		//TODO (print P/R)
	}

}

// =-=-=-=-= MAIN =-=-=-=-=

void help() {
	cout << "\n=-= EXERCISE 2 TDCV =-=\n"
		"\nThis program computes tasks related to exercise 2 of the WS2017-18 TDCV course at TUM\n"
		"(about random forests of HOG)\n"
		"Possible modes are:\n"
			"\t0- Data augmentation\n"
            "\t1- Task 1, compute and display HOG of an image\n"
            "\t2- Task 2, train and test a random forest on some data\n"
            "\t3- Task 3 --TO BE COMPLETED\n"
		"\nUsage and examples\n"
		"  ./ex2 [-hvi][--help][--verbose][--interactive][mode][filename]\n"
		"  ./ex2 1 data/task1/obj1000.jpg\n"
		"  ./ex2 -v 2 data/task2\n"
		"\nDetails\n"
		"\tAuthors Lapostre Thomas & Zhu Yue"
		"\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}


int main(int argc, char** argv)
{
	srand(time(NULL));
	//for(int i = 0; i<argc; i++)
	//	cout << argv[i] << " !" << endl;
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
		cout << "Verbose mode is on, yay!\n\n";
	}
	if (parser.has("interactive")) {
		interactive = true;
		cout << "Interactive mode is on, yay!\n\n";
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
			//testDisplayRects("test.jpg");
		}
		else if (task == 0) {
			data_aug(inputName, verbose);
		}
		else {
			cout << "Task number not existent or not implemented yet" << endl;
			return -1;
		}
		redo = false;
		if (interactive) {
			int q = inputNumber("Want to try again? (yes=1) ");
			redo = (q == 1);
		}
	}
	

	waitKey(0); // Wait for a keystroke in the window
	return 0;
}

