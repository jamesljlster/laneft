#ifndef LANEFT_OCV_HPP_INCLUDED
#define LANEFT_OCV_HPP_INCLUDED

#include <opencv2/opencv.hpp>

#include "laneft.hpp"

class laneft_ocv : public laneft
{
   public:
    double get_feature(cv::Mat src);
    void draw_line_onto(cv::Mat& dst);

    void set_find_point_rule(int maskSize, int threshold);
    void set_find_line_rule(int maxDist, int threshold);
    void set_line_height_filter(int threshold);

   protected:
    cv::Scalar get_order_color(int order);
};

#endif  // LANEFT_OCV_HPP_INCLUDED
