#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    cout << "Hello world!" << endl;
    Mat imageMat = imread("./resources/textures/1000.png");
    namedWindow("figure1", WINDOW_AUTOSIZE);
    imshow("figure1", imageMat);
    waitKey(0);
    return 0;
}