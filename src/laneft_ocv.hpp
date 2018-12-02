#ifndef LANEFT_OCV_HPP_INCLUDED
#define LANEFT_OCV_HPP_INCLUDED

#include <opencv2/opencv.hpp>

#include "laneft.hpp"

class laneft_ocv : public laneft
{
   public:
    laneft_ocv() : laneft() {}
    laneft_ocv(laneft::LANE_TYPE laneType) : laneft(laneType) {}

    double get_feature(cv::Mat src);
    void draw_line_onto(cv::Mat& dst);

   protected:
    cv::Scalar get_order_color(int order);
};

#endif  // LANEFT_OCV_HPP_INCLUDED
