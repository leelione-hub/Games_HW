#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN) 
    {
        if (control_points.size() < 13) 
        {
            std::cout << "Left button of the mouse is clicked - position (" << x << ", "
                << y << ")" << '\n';
            control_points.emplace_back(x, y);
        }
        else
        {
            std::cout << "Left button of the mouse is clicked but control_points size equal 13 " << '\n';
        }
    }

    if (event == cv::EVENT_RBUTTONDOWN && control_points.size() > 0) 
    {
        std::cout << "Right button of the mouse is clicked ontrol_points.size: "<< control_points.size() << '\n';
        control_points.erase(control_points.end() - 1);
    }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x) = cv::Vec3b(0, 255, 0);
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    //if (control_points.size() == 1)
    //{
    //    return control_points[0];
    //}
    //std::vector<cv::Point2f> newcontrol_points;
    //for (int i = 0; i < control_points.size() - 1; i++) 
    //{
    //    newcontrol_points.emplace_back(control_points[i] * t + (1.0 - t) * control_points[i + 1]);
    //}
    //return recursive_bezier(newcontrol_points, t);

    //使用伯恩斯坦多项式
    int n = control_points.size() - 1;
    std::vector<int> factorial(n + 1, 1);
    //1.记录下所有的阶乘的结果
    for (int i = 1; i <= n; i++) 
    {
        factorial[i] = factorial[i - 1] * i;
    }
    cv::Point2f point = cv::Point2f(0, 0);
    for (int i = 0; i <= n; i++) 
    {
        point += control_points[i] * factorial[n] / (factorial[i] * factorial[n - i]) * std::pow(t, i) * std::pow(1.0 - t, n - i);
    }
    return point;

    //// TODO: Implement de Casteljau's algorithm
    //return cv::Point2f();

}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.


    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        cv::Point2f point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        window.setTo(cv::Scalar(0, 0, 0));
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() > 2) 
        {
            //naive_bezier(control_points, window);
               bezier(control_points, window);
            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
 /*           key = cv::waitKey(20);*/

            if (key == 27) 
            {
                return 0;
            }
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
