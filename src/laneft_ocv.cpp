
#include "laneft_ocv.hpp"
#include <opencv2/opencv.hpp>

double laneft_ocv::get_feature(cv::Mat src)
{
    return laneft::get_feature(src.ptr(), src.cols, src.rows);
}

void laneft_ocv::set_find_point_rule(int maskSize, int threshold)
{
    laneft::set_find_point_rule(maskSize, threshold);
}

void laneft_ocv::set_find_line_rule(int maxDist, int threshold)
{
    laneft::set_find_line_rule(maxDist, threshold);
}

void laneft_ocv::set_line_height_filter(int threshold)
{
    laneft::set_line_height_filter(threshold);
}

void laneft_ocv::draw_line_onto(cv::Mat& dst)
{
    unsigned int i, j;

    // Draw all point with line handle
    for(i = 0; i < lineHandle.size(); i++)
    {
        for(j = 0; j < lineHandle.at(i).size(); j++)
        {
            cv::circle(dst, cv::Point(lineHandle.at(i).at(j).x, lineHandle.at(i).at(j).y), 1, get_order_color(i), 1);
        }
    }
}

cv::Scalar laneft_ocv::get_order_color(int order)
{
    int r = 0;
    int g = 0;
    int b = 0;

    int mod = order % 3;

    switch(mod)
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
