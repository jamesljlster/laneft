#include <opencv2/opencv.hpp>

#include "laneft_ocv.hpp"

double laneft_ocv::get_feature(cv::Mat src)
{
    return laneft::get_feature(src.ptr(), src.cols, src.rows);
}

void laneft_ocv::draw_line_onto(cv::Mat& dst)
{
    unsigned int i, j;

    // Draw all point with line handle
    for (i = 0; i < this->lineHandle.size(); i++)
    {
        for (j = 0; j < this->lineHandle.at(i).size(); j++)
        {
            cv::circle(dst,
                       cv::Point(this->lineHandle.at(i).at(j).x,
                                 this->lineHandle.at(i).at(j).y),
                       1, this->get_order_color(i), 1);
        }
    }
}

cv::Scalar laneft_ocv::get_order_color(int order)
{
    int r = 0;
    int g = 0;
    int b = 0;

    int mod = order % 3;

    switch (mod)
    {
        case 0:
            r = 255;
            break;

        case 1:
            g = 255;
            break;

        case 2:
            b = 255;
            break;
    }

    return cv::Scalar(b, g, r);
}
