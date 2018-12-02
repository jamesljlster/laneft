#ifndef LANEFT_HPP_INCLUDED
#define LANEFT_HPP_INCLUDED

#include <vector>

class laneft
{
    struct POINT
    {
        int x;
        int y;

        bool operator>(const POINT& cmp) const;
        bool operator==(const POINT& cmp) const;
        bool operator<(const POINT& cmp) const;
    };

   public:
    enum LANE_TYPE
    {
        LANE,
        LINE
    };

    laneft();
    laneft(enum LANE_TYPE laneType);
    ~laneft();

    void set_lane_type(enum LANE_TYPE laneType);

    double get_feature(unsigned char* src, int srcWidth, int srcHeight);

    void set_find_point_rule(int maskSize, int threshold);
    void set_find_line_rule(int maxDist, int threshold);
    void set_line_height_filter(int threshold);

   protected:
    // Member functions
    void laneft_init(enum LANE_TYPE laneType);

    void clear_hash_space();
    void del_hash_space();
    void clear_line_handle();
    void del_line_handle();

    void find_point_list(unsigned char* src, int srcWidth, int srcHeight);
    void generate_line(std::vector<struct POINT>& line,
                       struct POINT startPoint);
    void find_line();
    void clean_line(int imgWidth);
    void line_height_filter();

    double lane_to_feature(int imgWidth);
    double line_to_feature(int imgWidth);

    int sum_mask(unsigned char* src, int srcWidth, int srcHeight, int row,
                 int col);

    // Member variables
    enum LANE_TYPE laneType = LANE_TYPE::LANE;

    std::vector<struct POINT> ptList;
    std::vector<std::vector<struct POINT> > hashSpace;
    std::vector<std::vector<struct POINT> > lineHandle;

    int hashRowStep;
    int hashColStep;

    int maskSize;
    int pointTh;
    int maxDist;
    int lineTh;
    int lineHeightTh;
};

#endif  // LANEFT_HPP_INCLUDED
