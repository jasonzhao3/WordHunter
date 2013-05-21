#include "opencv2/opencv.hpp"
//#include <opencv2/core/utility.hpp>
#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

static const Vec3b bcolors[] =
  {
    Vec3b(0,0,255),
    Vec3b(0,128,255),
    Vec3b(0,255,255),
    Vec3b(0,255,0),
    Vec3b(255,128,0),
    Vec3b(255,255,0),
    Vec3b(255,0,0),
    Vec3b(255,0,255),
    Vec3b(255,255,255)
  };

template <typename T1, typename T2>
struct less_second {
  typedef pair<T1, T2> type;
  bool operator ()(type const& a, type const& b) const {
    return a.second < b.second;
  }
};


static bool isNeighbour(Rect & rect1, Rect & rect2, int & cHeight, int & cWidth);
static void findCharSize(vector<Rect> & boundRect, int & cHeight,int & cWidth);
static void mergeBoundRect(vector<Rect> & boundRect,int & index1, int & index2);


int main(int argc, char** argv)
{
  Mat image, grayImage, bwImage, equalImage,  mserImage, outputImage;
  image = imread (argv[1], 1);
  int blkSize = atoi(argv[2]);
  if (argc != 3 || !image.data) {
    printf("Incorrect input. Please enter: executable + imageFileName + blockSize \n");
    return -1;
  }
  
  //  namedWindow("Display_Image", CV_WINDOW_AUTOSIZE);
  // imshow("Display_Image", image);
  // rgb2gray
  cvtColor(image, grayImage, CV_RGB2GRAY);
  // equalize the image
  equalizeHist(grayImage, equalImage);
  // gray2bw
  adaptiveThreshold(grayImage,bwImage, 255, ADAPTIVE_THRESH_MEAN_C,\
		    THRESH_BINARY_INV, blkSize, 10);
  
  //add bounding box
  cvtColor(bwImage, outputImage, CV_GRAY2RGB, 0);
  vector<vector<Point> > contours;
  findContours(bwImage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

  int numContours = contours.size();
  vector<Rect> boundRect(numContours);
  Scalar color = Scalar(0,0,255);

  for (int i = 0; i < numContours; i++) {
    boundRect[i] = boundingRect(contours[i]);
    //    rectangle(outputImage, boundRect[i], color, 1, 8, 0);
  }

  // estimate the width and height of each character
  int cHeight, cWidth;
  findCharSize(boundRect, cHeight, cWidth);
  // merge the neighbour rectangles
  for (int i = 0; i < numContours; i++) { //how to eliminate the nested for-loop?
    for (int j = i + 1; j < numContours; j++) {
      if (isNeighbour(boundRect[i], boundRect[j], cHeight, cWidth)) 
	mergeBoundRect(boundRect, i, j);
    }
  }
  int rectNum = boundRect.size();
  for (int i = 0; i < boundRect.size(); i++) {
      rectangle(outputImage, boundRect[i], color, 1, 8, 0);
  }
  // mser
  // Size s = image.size();
  // mserImage = Mat::zeros(s, CV_32F);
  // vector<vector<Point> > contours;
  // MSER(15, 5, 100000, 200, 0.05)(equalImage, contours);
  // int x = contours.size();
  // cout <<"the value of x is " <<  x <<pppppp endl;
  // for (int i = 0; i < contours.size(); i++) {
  //   const vector<Point> &r = contours[i];
  //   for (int j = 0; j < r.size(); j++) {
  //     Point pt = r[j];
  //     mserImage.at<double>(pt) = 255;
  //   }
  // }
					
  imwrite("gray_image.jpg", grayImage);
  imwrite("equal_image.jpg", equalImage);
  imwrite("bw_image.jpg", bwImage);
  imwrite("ouptut_image.jpg", outputImage);

  //  imwrite("mser_image.jpg", mserImage); 
  printf("image saved successfully.\n");
  return 0;
}


static bool isNeighbour(Rect & rect1, Rect & rect2, int & cHeight, int & cWidth) {
  float dy = abs(rect1.tl().y - rect2.tl().y); // maybe problematic with x and y
  float dx = abs(rect1.tl().x - rect2.tl().x);

  if (dy < 0.5 * cHeight || dx < cWidth) return true;
  else return false;
}


static void findCharSize(vector<Rect> & boundRect, int & cHeight,int & cWidth) {
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
  cHeight = sqrt(areaAvg) * 1.3;
  cWidth = sqrt(areaAvg) * 0.8;
}


static void mergeBoundRect(vector<Rect> & boundRect,int & index1, int & index2) {
  int x1 = boundRect[index1].tl().x;
  int x2 = boundRect[index2].tl().x; 
  int y1 = boundRect[index1].tl().y;
  int y2 = boundRect[index2].tl().y;

  int xTl = x1 > x2 ? x2 : x1;
  int yTl =  y1 > y2 ? y2 : y1; //y is reversed coordinate?
  
  x1 = boundRect[index1].br().x;
  x2 = boundRect[index2].br().x;
  y1 = boundRect[index1].br().y;
  y2 = boundRect[index2].br().y;
  
  int xBr = x1 > x2 ? x1 : x2;
  int yBr = y1 > y2 ? y1 : y2;
  // do we need to handle the memory issue to free the memory?

  boundRect.erase(boundRect.begin() + index1);
  boundRect.erase(boundRect.begin() + index2);
  boundRect.push_back(Rect(Point(xTl, yTl), Point(xBr, yBr)));
}
