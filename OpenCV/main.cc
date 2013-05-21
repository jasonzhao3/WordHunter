#include "opencv2/opencv.hpp"
//#include <opencv2/core/utility.hpp>
#include <stdio.h>
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

int main(int argc, char** argv)
{
  Mat image, grayImage, bwImage, equalImage,  mserImage;
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
  Size s = image.size();
  mserImage = Mat::zeros(s, CV_32F);

  std::vector<vector<Point> > contours;
  MSER(15, 5, 100000, 200, 0.05)(equalImage, contours);
  int x = contours.size();
  cout <<"the value of x is " <<  x << endl;
  for (int i = 0; i < contours.size(); i++) {
    const vector<Point> &r = contours[i];
    for (int j = 0; j < r.size(); j++) {
      Point pt = r[j];
      mserImage.at<double>(pt) = 255;
    }
  }
					
  imwrite("gray_image.jpg", grayImage);
  imwrite("equal_image.jpg", equalImage);
  imwrite("bw_image.jpg", bwImage);
  imwrite("mser_image.jpg", mserImage);
  printf("image saved successfully.\n");
  return 0;
}
