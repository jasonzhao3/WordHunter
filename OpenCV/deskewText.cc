#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/**
 * @function main
 */
static Mat deskewText(Mat & src)
{
  int threshold_value = 0;
  int threshold_type = THRESH_BINARY;;
  int const max_value = 255;
  int const max_type = 4;
  int const max_BINARY_value = 255;
  double const pad = 0.4;
  int top, bottom, left, right;
  int borderType;
  Scalar value; 
  Mat src, src_gray, dst;
  Mat adapt_img;
  Mat adapt_img_padded;

  /// Load an image
  //src = imread( argv[1], 1 );

  Size size = src.size();
  /// Convert the image to Gray
  //cvtColor( src, src_gray, CV_RGB2GRAY );

  vector<Vec4i> lines;

  //adaptiveThreshold( src_gray, adapt_img,  max_BINARY_value, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 31, 10);
  top = (int) (pad *adapt_img.rows);
  bottom = (int) (pad *adapt_img.rows);
  left = (int) (pad *adapt_img.rows);
  right = (int) (pad *adapt_img.rows);
  borderType = BORDER_CONSTANT;
  value = Scalar(0, 0, 0); // pad with all black
  //imshow("Before padding", adapt_img);
  copyMakeBorder (adapt_img, adapt_img_padded, top, bottom, left, right, borderType, value);
  //imshow("after padding", adapt_img_padded);
  HoughLinesP(adapt_img_padded, lines, 1, CV_PI/180, 100, size.width / 2.f, 20);

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
  std::cout << "rotation matrix = " << std::endl << " " << rot_mat << std::endl << std::endl;
  Mat rotated;
  warpAffine(adapt_img_padded, rotated, rot_mat, adapt_img_padded.size(), INTER_CUBIC);
  std::cout << box.center.x << ", " << box.center.y << std::endl;
  imshow("Rotated", rotated);

//  Size box_size = box.size;
//  if (box.angle < -45.) {
//          std::swap(box_size.width, box_size.height);
//  }
//          Mat cropped;
//          getRectSubPix(rotated, box_size, box.center, cropped);
//
//  imshow("Cropped", cropped);
  waitKey(0);
  //imwrite("out_img_drawing.jpg", rotated);
  return rotated;

}


/**
 * @function Threshold_Demo
 */
