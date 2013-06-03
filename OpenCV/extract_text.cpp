#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "OCR.h"  //opencv also in OCR.h

using namespace cv;
using namespace std;
double const PAD = 0.4;
float const ANGLE_THRESH = 5;

#define TESS_DATA_CONFIG "alphanumeric"
#define MAX_WORD_LENGTH 25
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
static int isMatch(string & sResult, string & wordToSearch);
static bool withinLengthRange(Rect & rectBox, int & widthLimit);
static vector<Rect> labelLettersWithBox(Mat &letterWithBox, vector<vector<Point> > & contours);
static void mergeBox(vector<Rect> & boundRect, int & cHeight, int & cWidth);
static void searchAndLabelWord(Mat & resultImage, Mat & bb_mask, Mat & wordWithBox, Mat & bwImageForTess, \
                       vector<Rect> & boundRect, string & wordToSearch, int & widthLimit);
static void deskewText(Mat & src, Mat & deskewedImg, Mat & color_src, Mat & color_deskew, Mat & rot_mat_inv, int & top, int & bottom, int & left, int & right, double & angle_degrees);
static int findEditDistance(const string str1, const string str2, int cutoff, int order);
int Min(int dis1, int dis2, int dis3);

int main(int argc, char** argv)
{
  Mat image, grayImage, bwImage, equalImage, deskewedImage, wordWithBox, resultImage, filteredImage;
  Mat  color_deskew, color_deskew_inv, color_deskew_inv_cropped, rot_mat_inv;
  int top, bottom, left, right;
  double angle_degrees;
  if (argc != 3) {
    printf("Incorrect input. Please enter: executable + imageFileName + wordToSearch\n");
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
  medianBlur(grayImage, filteredImage, 5);
  //filteredImage = grayImage;
  // equalize the image
  equalizeHist(grayImage, equalImage);
  // gray2bw
  adaptiveThreshold(filteredImage, bwImage, 255, ADAPTIVE_THRESH_MEAN_C,\
		    THRESH_BINARY_INV, blkSize, 10);
  deskewText(bwImage, deskewedImage, image, color_deskew, rot_mat_inv, top, bottom, left, right, angle_degrees);
   if (abs(angle_degrees) > ANGLE_THRESH) { 
          Mat bb_mask_inv;
          Mat bb_mask_inv_cropped; 
          Mat bb_mask(color_deskew.rows, color_deskew.cols, CV_8UC3, Scalar(0,0,0));

           //deskewedImage = bwImage;



          Mat bwImageForTess = deskewedImage.clone();
          //add bounding box
          cvtColor(deskewedImage, wordWithBox, CV_GRAY2RGB, 0);

          vector<Vec4i> hierarchy;
          vector<vector<Point> > contours;
          Mat contourImage;
          cvtColor(deskewedImage, contourImage, CV_GRAY2RGB, 0);  
          //findContours(deskewedImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
          findContours(deskewedImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
           for (int i = 0; i < contours.size(); i++) {
             drawContours(contourImage, contours, i, Scalar(255, 0, 0), 1, 8, hierarchy, 0, Point());
           }
           imwrite("contours.jpg", contourImage);

          Mat letterWithBox = wordWithBox.clone();

          vector<Rect> boundRect = labelLettersWithBox(letterWithBox, contours);
          //imwrite("immediate_after_letter_label.jpg", letterWithBox);
          // estimate the width and height of each character
          int cHeight, cWidth, widthLimit;
          float cArea;
          findCharSize(boundRect, cHeight, cWidth, cArea, widthLimit, numLetters);
          
          // merge and then clear already merged rectangles, and too large and too-small regions
          mergeBox(boundRect, cHeight, cWidth);
          clearNullRect(boundRect, cArea);

          //searchAndLabelWord(resultImage, wordWithBox, bwImageForTess, boundRect, wordToSearch, widthLimit);
          searchAndLabelWord(color_deskew, bb_mask, wordWithBox, bwImageForTess, boundRect, wordToSearch, widthLimit);
          // Rotate the image and the bounding box back after it's been labeled
          warpAffine(color_deskew, color_deskew_inv, rot_mat_inv, color_deskew.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);
          warpAffine(bb_mask, bb_mask_inv, rot_mat_inv, color_deskew.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);
          // remove the padding
          Rect crop_box(left, top, image.cols, image.rows);
          color_deskew_inv_cropped = color_deskew_inv(crop_box);  	
          bb_mask_inv_cropped = bb_mask_inv(crop_box);  	
          // output result
          //imwrite("result_image.jpg", resultImage);
          imwrite("bb_mask_inv_cropped.jpg", bb_mask_inv_cropped);
          imwrite("color_deskew_inv.jpg", color_deskew_inv);
          imwrite("result_image.jpg", color_deskew_inv_cropped);
          imwrite("./output/result_image.jpg", color_deskew_inv_cropped);
          imwrite("letter_with_bounding_box.jpg", letterWithBox);
          imwrite("word_with_bounding_box.jpg", wordWithBox);
  } else {
          Mat bb_mask_inv;
          Mat bb_mask_inv_cropped; 
          Mat bb_mask(bwImage.rows, bwImage.cols, CV_8UC3, Scalar(0,0,0));

           //deskewedImage = bwImage;



          Mat bwImageForTess = bwImage.clone();
          //add bounding box
          cvtColor(bwImage, wordWithBox, CV_GRAY2RGB, 0);

          vector<Vec4i> hierarchy;
          vector<vector<Point> > contours;
          Mat contourImage;
          cvtColor(bwImage, contourImage, CV_GRAY2RGB, 0);  
          //findContours(bwImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
          findContours(bwImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
//           for (int i = 0; i < contours.size(); i++) {
//             drawContours(contourImage, contours, i, Scalar(255, 0, 0), 1, 8, hierarchy, 0, Point());
//           }
//           imwrite("contours.jpg", contourImage);

          Mat letterWithBox = wordWithBox.clone();

          vector<Rect> boundRect = labelLettersWithBox(letterWithBox, contours);
          //imwrite("immediate_after_letter_label.jpg", letterWithBox);
          // estimate the width and height of each character
          int cHeight, cWidth, widthLimit;
          float cArea;
          findCharSize(boundRect, cHeight, cWidth, cArea, widthLimit, numLetters);
          
          // merge and then clear already merged rectangles, and too large and too-small regions
          mergeBox(boundRect, cHeight, cWidth);
          clearNullRect(boundRect, cArea);

          searchAndLabelWord(resultImage, bb_mask, wordWithBox, bwImageForTess, boundRect, wordToSearch, widthLimit);
          // output result
          //imwrite("result_image.jpg", resultImage);
          imwrite("bb_mask.jpg", bb_mask);
          imwrite("result_image.jpg", resultImage);
          imwrite("./output/result_image.jpg", resultImage);
          imwrite("letter_with_bounding_box.jpg", letterWithBox);
          imwrite("word_with_bounding_box.jpg", wordWithBox);
  }
  printf("image saved successfully.\n");
  waitKey(0);
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
	myFile << sResult + "\n";
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


static int isMatch(string &sResult, string &wordToSearch) {
  //may need change to use edit distance
  int dis = findEditDistance(sResult, wordToSearch, (int)wordToSearch.length() * 0.3, 0);
  float ratio = (float) dis / wordToSearch.length();
  cout << "Ratio between " << sResult << " and " << wordToSearch << " = " << dis << "/" <<wordToSearch.length() << " = " << ratio << endl;
  if (dis == 0) {
          cout << " Match is red"<< endl;
          return 0;
  }
  else if (ratio < 0.25 && dis > 0 ) {
          cout << " Match is blue"<< endl;
          return 1;
  }
  else if (ratio < 0.43 && dis > 0 ) {
          cout << " Match is green"<< endl;
          return 2;
  }
  //0.6667 is a super inacurate threshold
  else return -1;
}

static int findEditDistance(const string str1, const string str2, int cutoff, int order) {
    if (str1.compare(str2) == 0) return 0;
    if (order > cutoff) return cutoff + 1;  //the point is to make it bigger than cutoff

    if (str1.empty()) return str2.length();
    if (str2.empty())  return str1.length();   //cannot be str1 == NULL
    if (str1[0] == str2[0]) return findEditDistance(str1.substr(1), str2.substr(1), cutoff, order);
    if (str1[0] != str2[0]) {
       int dis1 = findEditDistance(str1, str2.substr(1), cutoff, order + 1) + 1;
       int dis2 = findEditDistance(str2, str1.substr(1), cutoff, order + 1) + 1;
       int dis3 = findEditDistance(str1.substr(1), str2.substr(1), cutoff, order + 1) + 1;
       return Min(dis1, dis2, dis3);
    }
    return -1; //error flag
}

int Min(int dis1, int dis2, int dis3) {
    int dis[3] = {dis1, dis2, dis3};
    int minTmp = dis1;
    for (int i = 1; i < 3; i++) {
       minTmp = minTmp < dis[i] ? minTmp : dis[i];
    }
    return minTmp;
}




static bool withinLengthRange(Rect & rectBox, int & widthLimit) {
  float width = rectBox.width;
  if (width > 0.65 * widthLimit && width < 1.2 * widthLimit) 
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


static void searchAndLabelWord(Mat & resultImage, Mat & bb_mask, Mat & wordWithBox, Mat & bwImageForTess, \
                       vector<Rect> & boundRect, string & wordToSearch, int & widthLimit) {
  int rectNum = boundRect.size();
  Scalar color = Scalar(0,0,255);
  for (int i = 0; i < rectNum; i++) {
      if (1) {
      //if (withinLengthRange(boundRect[i], widthLimit)) {
        Mat wordWindow(bwImageForTess, boundRect[i]);
        Mat wordWindowWithPadding = addPadding(wordWindow);
        //string wordWindowName = "word_" + to_string(i) + ".jpg";
        //imwrite(wordWindowName, wordWindowWithPadding);
        //string sResult = textRecognition(wordWindow);
        string sResult = textRecognition(wordWindowWithPadding);
        //writeFile(sResult);
        
        //only circle out the mached words
        int matchCode = isMatch(sResult, wordToSearch);
        if ( matchCode >= 0) {
           //exactl match: red
          if (matchCode == 0) {
           rectangle(resultImage, boundRect[i], color, 4, 8, 0);
           rectangle(bb_mask, boundRect[i], color, 4, 8, 0);
          }
          else if (matchCode == 1) {
           // not exactly match: blue
           rectangle(resultImage, boundRect[i], Scalar(255, 0, 0), 4, 8, 0);
           rectangle(bb_mask, boundRect[i], Scalar(255, 0, 0), 4, 8, 0);
          }
          else if (matchCode == 2) {
            //even further: green
           rectangle(resultImage, boundRect[i], Scalar(0, 255, 0), 4, 8, 0);
           rectangle(bb_mask, boundRect[i], Scalar(0, 255, 0), 4, 8, 0);
          }
        }
//      ostringstream convert; 
//      convert << i;
//      string fileNum = convert.str();
//      string fileName = "bounding" + fileNum + ".jpg";    
//      imwrite(fileName, wordWindowWithPadding);
      }
      rectangle(wordWithBox, boundRect[i], color, 1, 8, 0);
  }
}



static void deskewText(Mat & src, Mat & deskewedImg, Mat & color_src, Mat & color_deskew, Mat & rot_mat_inv, int & top, int & bottom, int & left, int & right, double & angle_degrees)
{
  int threshold_value = 0;
  int threshold_type = THRESH_BINARY;;
  int const max_value = 255;
  int const max_type = 4;
  int const max_BINARY_value = 255;
  int borderType;
  Scalar value; 
  
  Mat adapt_img = src;
  Mat adapt_img_padded;
  Mat deskewedImg_blurred;
  Mat color_src_padded;
  /// Load an image
  //src = imread( argv[1], 1 );

  Size size = src.size();
  /// Convert the image to Gray
  //cvtColor( src, src_gray, CV_RGB2GRAY );

  vector<Vec4i> lines;

  top = (int) (PAD *adapt_img.rows);
  bottom = (int) (PAD *adapt_img.rows);
  left = (int) (PAD *adapt_img.cols);
  right = (int) (PAD *adapt_img.cols);
  borderType = BORDER_CONSTANT;
  value = Scalar(0, 0, 0); // pad with all black
  //imshow("Before padding", adapt_img);

  copyMakeBorder (adapt_img, adapt_img_padded, top, bottom, left, right, borderType, value);
  copyMakeBorder (color_src, color_src_padded, top, bottom, left, right, IPL_BORDER_REPLICATE, value);
  //imshow("after padding", adapt_img_padded);
  HoughLinesP(adapt_img_padded, lines, 1, CV_PI/180, 100, size.width / 2.f, 20);

  Mat disp_lines(size, CV_8UC1, Scalar(0, 0, 0));
  double angle = 0.;
  unsigned nb_lines = lines.size();
  for (unsigned i = 0; i < nb_lines; ++i)
  {
      line(disp_lines, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));
      angle += atan2((double)lines[i][3] - lines[i][1],
                     (double)lines[i][2] - lines[i][0]);
  }
  angle /= nb_lines; // mean angle, in radians.
  angle_degrees = angle * 180/CV_PI; // mean angle, in radians.

  if (abs(angle_degrees) > ANGLE_THRESH) {
          printf ("angle is %f, need deskew\n", angle_degrees); 
          //imshow("Hough Lines", disp_lines);

          // Compute the bounding box of the entire text:
          vector<Point> points;
          Mat_<uchar>::iterator it = adapt_img_padded.begin<uchar>();
          Mat_<uchar>::iterator end = adapt_img_padded.end<uchar>();
          for (; it != end; ++it) {
            if (*it) {
              points.push_back(it.pos());
            }
          }
          RotatedRect box = minAreaRect(Mat(points));
          Mat rot_mat = getRotationMatrix2D(box.center, angle_degrees, 1);
          invertAffineTransform(rot_mat, rot_mat_inv);
          std::cout << "rotation matrix = " << std::endl << " " << rot_mat << std::endl << std::endl;

          Mat se = getStructuringElement(MORPH_ELLIPSE, Size(3,3));  
          Mat color_deskew_inv; 
          //warpAffine(adapt_img, deskewedImg_blurred, rot_mat, adapt_img.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);
          //warpAffine(color_src, color_deskew, rot_mat, color_src.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);
          warpAffine(adapt_img_padded, deskewedImg_blurred, rot_mat, adapt_img_padded.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);
          warpAffine(color_src_padded, color_deskew, rot_mat, color_src_padded.size(), INTER_LANCZOS4, BORDER_TRANSPARENT);

          std::cout << box.center.x << ", " << box.center.y << std::endl;
          //imshow("after deskew", deskewedImg);
          threshold(deskewedImg_blurred, deskewedImg, 128, 255, THRESH_BINARY);
          // waitKey(0);
          imwrite("color_src_padded.jpg", color_src_padded);
          imwrite("src_image.jpg", src);
          imwrite("deskewed_image.jpg", deskewedImg);
          imwrite("color_image_deskew.jpg", color_deskew);
  } else {
          cout << "angle is " << angle_degrees << " No need for deskew"<< endl;
  }

}
