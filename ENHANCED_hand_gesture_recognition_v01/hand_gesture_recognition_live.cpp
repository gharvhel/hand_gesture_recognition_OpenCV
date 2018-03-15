// =======================================================================
//       Filename:  hand_gesture_recognition_live.cpp
//    Description:  A program which will take as its input a sequence of
//                  live images of either a fist or a palm and then
//                  produce from such sequence an instruction to the
//                  computer.
//        Version:  1.0
//        Created:  03/15/2018 02:04:20 AM
//       Revision:  none
//       Compiler:  clang
//         Author:  Gharvhel Carre (), gc2767@columbia.edu
//   Organization:  Columbia University
// =======================================================================

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#define FIST 1
#define PALM 2

using namespace std;
using namespace cv;

void die(const char *msg);
void create_mask(Mat & image, Mat & image_mask, const char *w_name);
void load_images(const char *image_file_name, Mat & image);
void find_dimensions(Mat & image, int &image_width, int &image_height);
void find_largest_contour(vector < vector < Point >> &contours,
                          int &largest_contour_index,
                          int &largest_coutour);
int get_quadrant(int &center_x, int &center_y, Mat & image, int &where,
                 char *what_and_where);

int const LOW_RED = 80, HIGH_RED = 255, LOW_GREEN = 50;
int const HIGH_GREEN = 255, LOW_BLUE = 0, HIGH_BLUE = 20;

int main(int argc, char **argv) {
  VideoCapture camera(0);
  if (!camera.isOpened()) {
    die("ERROR: could not opened camera");
  }
  // Variables
  Mat image1, image2, combined_images, image1_cpy, image2_cpy;
  Mat image1_mask, image2_mask, combined_image_masks, combined_result,
    combined_result2, combined_result3;
  int image1_width, image1_height, image2_width, image2_height;
  int what1, where1, what2, where2;
  int frame = 1;
  namedWindow("RESULT", WINDOW_NORMAL);

  while (1) {
    cout << "FRAME: " << frame << endl;
    if (frame == 1) {
      if (camera.read(image1) == false) {
        die("ERROR: cannot read image");
      }
      // Get dimension of images
      find_dimensions(image1, image1_width, image1_height);

      // Create image mask based on skin color
      create_mask(image1, image1_mask, "FRAME1");

      // find contours
      vector < vector < Point >> contours1;
      vector < Vec4i > hierarchy1;
      findContours(image1_mask, contours1, hierarchy1, CV_RETR_EXTERNAL,
                   CV_CHAIN_APPROX_SIMPLE, Point());

      // find largest contour
      int largest_contour1_index = -1;
      int largest_coutour1 = 0;
      find_largest_contour(contours1, largest_contour1_index,
                           largest_coutour1);

      // Get the moments to calculate some features like center of mass
      Moments m1;
      m1 = moments(contours1[largest_contour1_index], false);

      //  Get the mass centers:
      int image1_center_x = m1.m10 / m1.m00;
      int image1_center_y = m1.m01 / m1.m00;

      // Find convex hull, and defects
      vector < vector < int >>hull1(contours1.size());
      vector < vector < Point >> hullPoint1(contours1.size());
      vector < vector < Vec4i >> defects1(contours1.size());
      vector < vector < Point >> defectPoint1(contours1.size());

      int count1 = 0;           // to count fingers
      int quadrant1;

      for (int i = 0; i < contours1.size(); i++) {
        if (contourArea(contours1[i]) > 5000) {
          convexHull(contours1[i], hull1[i], true);
          convexityDefects(contours1[i], hull1[i], defects1[i]);
          if (largest_contour1_index == i) {
            for (int j = 0; j < hull1[i].size(); j++) {
              int ind = hull1[i][j];
              hullPoint1[i].push_back(contours1[i][ind]);
            }

            // first defect is usually wrong; so skip it
            bool skiped_first = false;

            for (int j = 0; j < defects1[i].size(); j++) {
              if (defects1[i][j][3] > 11500) {
                int end_point = defects1[i][j][1];
                int far_point = defects1[i][j][2];
                defectPoint1[i].push_back(contours1[i][far_point]);
                if (skiped_first) {
                  circle(image1, contours1[i][end_point], 40,
                         Scalar(50, 0, 255), -1);
                  count1++;
                }
                skiped_first = true;
              }
            }

            // if count is less than 2, then assume is a fist, else palm
            char what_and_where[50];
            if (count1 <= 1) {
              strcpy(what_and_where, "(FIST, ");
              what1 = 1;
            } else {
              strcpy(what_and_where, "(PALM, ");
              what1 = 2;
            }

            quadrant1 =
              get_quadrant(image1_center_x, image1_center_y, image1,
                           where1, what_and_where);

            // draw contour, convex hull
            // write the where and what
            drawContours(image1, contours1, i, Scalar(130, 0, 75), 10, 8,
                         vector < Vec4i > (), 0, Point());
            drawContours(image1, hullPoint1, i, Scalar(0, 0, 255), 10, 8,
                         vector < Vec4i > (), 0, Point());
            circle(image1, Point(image1_center_x, image1_center_y), 40,
                   Scalar(255, 0, 0), -1, 8, 0);
            putText(image1, what_and_where,
                    Point(image1_width / 3, image1_width / 5),
                    CV_FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 0, 255), 5);
            putText(image1, to_string(count1),
                    Point(image1_width / 5, image1_height / 5),
                    CV_FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 0, 255), 5);
          }
        }
      }
      ++frame;
    } else if (frame == 3) {
      if (camera.read(image2) == false) {
        cout << "ERROR: cannot read image" << endl;
        return -1;
      }
      // Get dimension of images
      find_dimensions(image2, image2_width, image2_height);

      // Create image mask based on skin color
      create_mask(image2, image2_mask, "FRAME2");

      // find contours
      vector < vector < Point >> contours2;
      vector < Vec4i > hierarchy2;
      findContours(image2_mask, contours2, hierarchy2, CV_RETR_EXTERNAL,
                   CV_CHAIN_APPROX_SIMPLE, Point());

      // find largest contour
      int largest_contour2_index = -1;
      int largest_coutour2 = 0;
      find_largest_contour(contours2, largest_contour2_index,
                           largest_coutour2);

      // Get the moments to calculate some features like center of mass
      Moments m2;
      m2 = moments(contours2[largest_contour2_index], false);

      //  Get the mass centers:
      int image2_center_x = m2.m10 / m2.m00;
      int image2_center_y = m2.m01 / m2.m00;

      // Find convex hull, and defects
      vector < vector < int >>hull2(contours2.size());
      vector < vector < Point >> hullPoint2(contours2.size());
      vector < vector < Vec4i >> defects2(contours2.size());
      vector < vector < Point >> defectPoint2(contours2.size());

      int count2 = 0;           // to count fingers
      int quadrant2;

      for (int i = 0; i < contours2.size(); i++) {
        if (contourArea(contours2[i]) > 5000) {
          convexHull(contours2[i], hull2[i], true);
          convexityDefects(contours2[i], hull2[i], defects2[i]);
          if (largest_contour2_index == i) {
            for (int j = 0; j < hull2[i].size(); j++) {
              int ind = hull2[i][j];
              hullPoint2[i].push_back(contours2[i][ind]);
            }

            // first defect is usually wrong; so skip it
            bool skiped_first = false;

            for (int j = 0; j < defects2[i].size(); j++) {
              if (defects2[i][j][3] > 11500) {
                int end_point = defects2[i][j][1];
                int far_point = defects2[i][j][2];
                defectPoint2[i].push_back(contours2[i][far_point]);
                if (skiped_first) {
                  circle(image2, contours2[i][end_point], 40,
                         Scalar(50, 0, 255), -1);
                  count2++;
                }
                skiped_first = true;
              }
            }

            // if count is less than 2, then assume is a fist, else palm
            char what_and_where[50];
            if (count2 <= 1) {
              strcpy(what_and_where, "(FIST, ");
              what2 = 1;
            } else {
              strcpy(what_and_where, "(PALM, ");
              what2 = 2;
            }

            quadrant2 =
              get_quadrant(image2_center_x, image2_center_y, image2,
                           where2, what_and_where);

            // draw contour, convex hull
            // write the where and what
            drawContours(image2, contours2, i, Scalar(130, 0, 75), 10, 8,
                         vector < Vec4i > (), 0, Point());
            drawContours(image2, hullPoint2, i, Scalar(0, 0, 255), 10, 8,
                         vector < Vec4i > (), 0, Point());
            circle(image2, Point(image2_center_x, image2_center_y), 40,
                   Scalar(255, 0, 0), -1, 8, 0);
            putText(image2, what_and_where,
                    Point(image2_width / 3, image2_width / 5),
                    CV_FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 0, 255), 5);
            putText(image2, to_string(count2),
                    Point(image2_width / 5, image2_height / 5),
                    CV_FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 0, 255), 5);
          }
        }
      }

      // display results
      hconcat(image1, image2, combined_result);
      imshow("RESULT", combined_result);

      if (what1 == PALM && what2 == FIST) {
        if (where1 == 5 && where2 == 5)
          cout << "SELECTED\n";
        else
          cout << "UNKNOWN\n";
      } else if (what1 == FIST && what2 == PALM) {
        if (where1 != 5) {
          cout << "UNKNOWN\n";
        } else if (where2 == 1) {
          cout << "MOVED TO UPPER LEFT\n";
        } else if (where2 == 2) {
          cout << "MOVED TO UPPER MIDDLE\n";
        } else if (where2 == 3) {
          cout << "MOVED TO UPPER RIGHT\n";
        } else if (where2 == 4) {
          cout << "MOVED TO LEFT\n";
        } else if (where2 == 5) {
          cout << "CANCELLED\n";
        } else if (where2 == 6) {
          cout << "MOVED TO RIGHT\n";
        } else if (where2 == 7) {
          cout << "MOVED TO LOWER LEFT\n";
        } else if (where2 == 8) {
          cout << "MOVED TO LOWER MIDDLE\n";
        } else if (where2 == 9) {
          cout << "MOVED TO LOWER RIGHT\n";
        }
      } else
        cout << "UNKNOWN\n";

      ++frame;
    } else if (frame == 2) {
      waitKey(1000);
      ++frame;
    } else if (frame == 4) {
      waitKey(1000);
      frame = 1;
    }
  }
  return 0;
}

int get_quadrant(int &center_x, int &center_y, Mat & image, int &where,
                 char *what_and_where) {
  int y_quadrant = image.size().height / 3;
  int x_quadrant = image.size().width / 3;
  int quadrant;

  if ((center_y <= y_quadrant) && (center_x <= x_quadrant)) {
    strcat(what_and_where, "UPPER LEFT)");
    quadrant = 1;
  } else if ((center_y <= 2 * y_quadrant) && (center_x <= x_quadrant)) {
    strcat(what_and_where, "MIDDLE LEFT)");
    quadrant = 4;
  } else if ((center_y <= 3 * y_quadrant) && (center_x <= x_quadrant)) {
    strcat(what_and_where, "LOWER LEFT)");
    quadrant = 7;
  } else if ((center_y <= y_quadrant) && (center_x <= 2 * x_quadrant)) {
    strcat(what_and_where, "UPPER MIDDLE)");
    quadrant = 2;
  } else if ((center_y <= 2 * y_quadrant) && (center_x <= 2 * x_quadrant)) {
    strcat(what_and_where, "CENTER)");
    quadrant = 5;
  } else if ((center_y <= 3 * y_quadrant) && (center_x <= 2 * x_quadrant)) {
    strcat(what_and_where, "LOWER MIDDLE)");
    quadrant = 8;
  } else if ((center_y <= y_quadrant) && (center_x <= 3 * x_quadrant)) {
    strcat(what_and_where, "UPPER RIGHT)");
    quadrant = 3;
  } else if ((center_y <= 2 * y_quadrant) && (center_x <= 3 * x_quadrant)) {
    strcat(what_and_where, "MIDDLE RIGHT)");
    quadrant = 6;
  } else if ((center_y <= 3 * y_quadrant) && (center_x <= 3 * x_quadrant)) {
    strcat(what_and_where, "LOWER RIGHT)");
    quadrant = 9;
  }
  where = quadrant;
  return quadrant;
}

void find_largest_contour(vector < vector < Point >> &contours,
                          int &largest_contour_index,
                          int &largest_coutour) {
  for (int i = 0; i < contours.size(); i++) {
    if (contours[i].size() > largest_coutour) {
      largest_coutour = contours[i].size();
      largest_contour_index = i;
    }
  }
}

void find_dimensions(Mat & image, int &image_width, int &image_height) {
  image_width = image.size().width;
  image_height = image.size().height;
}

void load_images(const char *image_file_name, Mat & image) {
  image = imread(image_file_name, IMREAD_COLOR);

  if (image.empty()) {
    die("error opening file1\n");
  }
}

void die(const char *msg) {
  perror(msg);
  exit(1);
}

void create_mask(Mat & image, Mat & image_mask, const char *w_name) {
  // convert to HSV
  cvtColor(image, image_mask, COLOR_BGR2HSV);

  // pick out skin color
  inRange(image_mask, Scalar(LOW_BLUE, LOW_GREEN, LOW_RED),
          Scalar(HIGH_BLUE, HIGH_GREEN, HIGH_RED), image_mask);

  // erode
  erode(image_mask, image_mask, MORPH_ELLIPSE);

  //dilate
  dilate(image_mask, image_mask, MORPH_ELLIPSE);

  // blur to remove noise
  GaussianBlur(image_mask, image_mask, Size(19, 19), 0.0, 0);
}
