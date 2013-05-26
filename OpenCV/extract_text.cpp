#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "OCR.h"  //opencv also in OCR.h

using namespace cv;
using namespace std;


#define TESS_DATA_CONFIG "alphanumeric"
// the  name corresponds to the configs files in the tessdata/config 

//comparator used for rectangle comparation
template <typename T1, typename T2>
struct less_second {
  typedef pair<T1, T2> type;
  bool operator ()(type const& a, type const& b) const {
    return a.second < b.second;
  }
};

//change number to string
template <class T>
static inline string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}


static bool isNeighbour(Rect & rect1, Rect & rect2, int & cHeight, int & cWidth);
static void findCharSize(vector<Rect> & boundRect, int & cHeight,int & cWidth, \
                        float & cArea, int & widthLimit, int & numLetters);
static void mergeBoundRect(vector<Rect> & boundRect,int & index1, int & index2);
static void clearNullRect(vector<Rect> & boundRect, float & cArea);
static string textRecognition(Mat & txtImage);
static void writeFile(string & sResult);
static Mat addPadding(Mat & wordWindow);
static bool isMatch(string & sResult, string & wordToSearch);
static bool withinLengthRange(Rect & rectBox, int & widthLimit);
static vector<Rect> labelLettersWithBox(Mat &letterWithBox, vector<vector<Point> > & contours);
static void mergeBox(vector<Rect> & boundRect, int & cHeight, int & cWidth);
static void searchAndLabelWord(Mat & resultImage, Mat & wordWithBox, Mat & bwImageForTess, \
                       vector<Rect> & boundRect, string & wordToSearch, int & widthLimit);



int main(int argc, char** argv)
{
  Mat image, grayImage, bwImage, equalImage, wordWithBox, resultImage;
  if (argc != 3) {
    printf("Incorrect input. Please enter: executable + imageFileName + wordToSearch \n");
    return -1;
  }
  //get input information
  image = imread (argv[1], 1);
  string wordToSearch = argv[2];

  //copy the image, and parameters(i.e. size) set-up
  resultImage = image.clone();
  int numLetters = wordToSearch.length();
  int blkSize = 25;
  
  // rgb2gray
  cvtColor(image, grayImage, CV_RGB2GRAY);
  // equalize the image
  equalizeHist(grayImage, equalImage);
  // gray2bw
  adaptiveThreshold(grayImage,bwImage, 255, ADAPTIVE_THRESH_MEAN_C,\
		    THRESH_BINARY_INV, blkSize, 10);
  
  Mat bwImageForTess = bwImage.clone();
  //add bounding box
  cvtColor(bwImage, wordWithBox, CV_GRAY2RGB, 0);
  vector<vector<Point> > contours;
  findContours(bwImage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  
  Mat letterWithBox = wordWithBox.clone();

  vector<Rect> boundRect = labelLettersWithBox(letterWithBox, contours);

  // estimate the width and height of each character
  int cHeight, cWidth, widthLimit;
  float cArea;
  findCharSize(boundRect, cHeight, cWidth, cArea, widthLimit, numLetters);
  
  // merge and then clear already merged rectangles, and too large and too-small regions
  mergeBox(boundRect, cHeight, cWidth);
  clearNullRect(boundRect, cArea);

  searchAndLabelWord(resultImage, wordWithBox, bwImageForTess, boundRect, wordToSearch, widthLimit);
	
  // output result
  imwrite("result_image.jpg", resultImage);
  imwrite("letter_with_bounding_box.jpg", letterWithBox);
  imwrite("word_with_bounding_box.jpg", wordWithBox);
  printf("image saved successfully.\n");
  return 0;
}


static bool isNeighbour(Rect & rect1, Rect & rect2, int & cHeight, int & cWidth) {
  if (rect1.area() == 0 || rect2.area() == 0) return false;

  float dy1 = abs(rect1.tl().y - rect2.tl().y); //i and j's dot
  float dy2 = abs(rect1.br().y - rect2.br().y); //"al"'s problem
  float dx1 = abs(rect1.tl().x - rect2.br().x); //if rect2 is in front of rect1
  float dx2 = abs(rect1.br().x - rect2.tl().x);
  //two dx is because when the bounding box becomes a rectangule, the original dx will not work anymore
  //two rectangles intersect
  if ((rect1 & rect2).area() != 0 ) return true;
  if ((dy1 < 0.25 * cHeight || dy2 < 0.35 * cHeight) && (dx1 < 0.45 * cWidth || dx2 < 0.45 *cWidth)) return true;
  else return false;
}


static void findCharSize(vector<Rect> & boundRect, int & cHeight, \
                      int & cWidth, float & cArea, int & widthLimit, int & numLetters) {
  map<int, int> areaMap;	// key: area_value  value: frequency
  int numRects = boundRect.size();
  for(int i = 0; i < numRects; i++) {
    int area = boundRect[i].area();
    areaMap[area] += 1;
  }
  
  vector< pair<int, int> > areaArray(areaMap.begin(), areaMap.end());
  sort(areaArray.begin(), areaArray.end(), less_second<int, int>());
  float areaAvg;
  for (int i = 0; i < 10; i++) {
    areaAvg += areaArray[i].first;
  }
  areaAvg = areaAvg / 10.0;
  cHeight = sqrt(areaAvg) * 1.1;
  cWidth = sqrt(areaAvg) * 0.75;
  cArea = areaAvg;
  widthLimit = numLetters * cWidth;
 // cout << "character width is " << cWidth << endl;
 // cout << "character height is " << cHeight << endl;
}


static void mergeBoundRect(vector<Rect> & boundRect,int & index1, int & index2) {
   // // do we need to handle the memory issue to free the memory?
  Rect newRect = boundRect[index1] | boundRect[index2];
  Rect nullRect = Rect(Point(0,0), Point(0,0));
  boundRect[index1] = nullRect;
  boundRect[index2] = nullRect;
  boundRect.push_back(newRect);
}


static void clearNullRect(vector<Rect> & boundRect, float & cArea){
  for (int i = 0; i < boundRect.size(); i++) {
      float area = boundRect[i].area();
    if (area == 0 || area < 0.35 * cArea || area > 15 * cArea) {
      boundRect.erase(boundRect.begin() + i);
      --i;
    }
  }
}

static string textRecognition(Mat & txtImage) {
	// Perform the recognition
  IplImage inputImage = txtImage;
	TextReader tReader( TESS_DATA_CONFIG);  
	return tReader.RecognizePatch( &inputImage);
}

static void writeFile(string & sResult) {
	// Output results
	ofstream myFile;
	myFile.open("output.txt", ios::app);
	myFile << sResult;
	myFile.close();
}

static Mat addPadding(Mat & wordWindow) {
  Mat wordWindowWithPadding;
  int top, bottom, left, right;
  top = (int) wordWindow.rows;
  bottom = (int) wordWindow.rows;
  left = (int) wordWindow.cols;
  right = (int) wordWindow.cols;
  
  // cout << "top is " << top << endl;
  // cout << "left is " << top << endl;
  
  copyMakeBorder(wordWindow, wordWindowWithPadding, \
    top, bottom, left, right, BORDER_CONSTANT, Scalar(0, 0, 0));
  return wordWindowWithPadding;
}


static bool isMatch(string &sResult, string &wordToSearch) {
  //may need change to use edit distance
  if (wordToSearch.compare(sResult) == 0) 
    return true;
  else return false;
}

static bool withinLengthRange(Rect & rectBox, int & widthLimit) {
  float width = rectBox.width;
  if (width > 0.8 * widthLimit && width < 1.2 * widthLimit) 
    return true;
  else 
    return false;
}

static vector<Rect> labelLettersWithBox(Mat &letterWithBox, vector<vector<Point> > & contours) {
  int numContours = contours.size();
  vector<Rect> boundRect(numContours);
  Scalar color = Scalar(0,0,255);
  for (int i = 0; i < numContours; i++) {
    boundRect[i] = boundingRect(contours[i]);
    rectangle(letterWithBox, boundRect[i], color, 1, 8, 0);
  }
  return boundRect;
}

static void mergeBox(vector<Rect> & boundRect, int & cHeight, int & cWidth) {
  // merge the neighbour rectangles
  for (int i = 0; i < boundRect.size(); i++) { //how to eliminate the nested for-loop?
    for (int j = i+1; j < boundRect.size(); j++) {
      if (isNeighbour(boundRect[i], boundRect[j], cHeight, cWidth)) {
         mergeBoundRect(boundRect, i, j);
         break;  //just break from the inner for-loop to enhance the efficiency
      } 
    }
  }
}


static void searchAndLabelWord(Mat & resultImage, Mat & wordWithBox, Mat & bwImageForTess, \
                       vector<Rect> & boundRect, string & wordToSearch, int & widthLimit) {
  int rectNum = boundRect.size();
  Scalar color = Scalar(0,0,255);
  for (int i = 0; i < rectNum; i++) {
      if (withinLengthRange(boundRect[i], widthLimit)) {
        Mat wordWindow(bwImageForTess, boundRect[i]);
        Mat wordWindowWithPadding = addPadding(wordWindow);
        string sResult = textRecognition(wordWindowWithPadding);

        //only circle out the mached words
        if (isMatch(sResult, wordToSearch)) {
           rectangle(resultImage, boundRect[i], color, 1, 8, 0);
        }
      }
      //writeFile(sResult);
      rectangle(wordWithBox, boundRect[i], color, 1, 8, 0);
  }
}