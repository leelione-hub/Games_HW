#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

using namespace std;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f rotate;
    float angle = rotation_angle / 180.0 * MY_PI;
    rotate << cos(angle), -sin(angle), 0, 0,
        sin(angle), cos(angle), 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1;

    model = rotate * model;

    return model;
}

Eigen::Matrix4f get_rote_matrix(Eigen::Vector3f axis, float rote_angle)
{
    float angle = rote_angle / 180.0 * MY_PI;
    Eigen::Matrix4f R;
    Eigen::Matrix3f r;
    Eigen::Matrix3f m;
    m << 0, -axis[2], axis[1],
        axis[2], 0, -axis[0],
        -axis[2], axis[0], 0;
    r = Eigen::Matrix3f::Identity() * std::cos(angle) + (1 - std::cos(angle)) * axis * axis.adjoint() + std::sin(angle) * m;
    R << r(0, 0), r(0, 1), r(0, 2), 0,
        r(1, 0), r(1, 1), r(1, 2), 0,
        r(2, 0), r(2, 1), r(2, 2), 0,
        0, 0, 0, 1;

    return R;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float halffov = eye_fov / 2.0 / 180.0 * MY_PI;
    float t = zNear * tan(halffov);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    Eigen::Matrix4f translate;
    translate << zNear / r, 0, 0, 0,
        0, zNear / t, 0, 0,
        0, 0, -(zNear + zFar) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
        0, 0, -1, 0;
    projection = translate * projection;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    Eigen::Vector3f axis;
    axis << 2, 0, 0;

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_rote_matrix(axis,angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
