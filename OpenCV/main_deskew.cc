#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables

int threshold_value = 0;
int threshold_type = THRESH_BINARY;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst;
Mat adapt_img;
/// Function headers
void Threshold_Demo( int, void* );

/**
 * @function main
 */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1], 1 );

  Size size = src.size();
  /// Convert the image to Gray
  cvtColor( src, src_gray, CV_RGB2GRAY );

  vector<Vec4i> lines;
  adaptiveThreshold( src_gray, adapt_img,  max_BINARY_value, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 31, 10);
  // save a version of the tresholded image in RGB
  HoughLinesP(adapt_img, lines, 1, CV_PI/180, 100, size.width / 2.f, 20);

  Mat disp_lines(size, CV_8UC1, Scalar(0, 0, 0));
  double angle = 0.;
  double angle_degrees = 0.;
  unsigned nb_lines = lines.size();
  for (unsigned i = 0; i < nb_lines; ++i)
  {
      line(disp_lines, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));
      angle += atan2((double)lines[i][3] - lines[i][1],
                     (double)lines[i][2] - lines[i][0]);
  }
  angle /= nb_lines; // mean angle, in radians.
  angle_degrees = angle * 180/CV_PI; // mean angle, in radians.

  printf ("angle is %f\n", angle_degrees); 
  namedWindow("Hough Lines", CV_WINDOW_AUTOSIZE); 
  imshow("Hough Lines", disp_lines);

  // Compute the bounding box of the entire text:
  vector<Point> points;
  Mat_<uchar>::iterator it = adapt_img.begin<uchar>();
  Mat_<uchar>::iterator end = adapt_img.end<uchar>();
  for (; it != end; ++it) {
    if (*it) {
      points.push_back(it.pos());
    }
  }
  RotatedRect box = minAreaRect(Mat(points));
  Mat rot_mat = getRotationMatrix2D(box.center, angle_degrees, 1);
  std::cout << "rotation matrix = " << std::endl << " " << rot_mat << std::endl << std::endl;
  Mat rotated;
  warpAffine(adapt_img, rotated, rot_mat, adapt_img.size(), INTER_CUBIC);
  std::cout << box.center.x << ", " << box.center.y << std::endl;
  namedWindow("Rotated", CV_WINDOW_AUTOSIZE); 
  imshow("Rotated", rotated);

  waitKey(0);
  //imwrite("out_img_drawing.jpg", adapt_img);
  return 0;

}


/**
 * @function Threshold_Demo
 */
