#ifndef LANEFT_HPP_INCLUDED
#define LANEFT_HPP_INCLUDED

#include <vector>

class laneft
{
    struct POINT
    {
        int x;
        int y;

        bool operator>(const POINT& cmp) const
        {
            if (this->y > cmp.y)
            {
                return true;
            }
            else if (this->y == cmp.y)
            {
                if (this->x > cmp.x)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        bool operator==(const POINT& cmp) const
        {
            if (this->y == cmp.y && this->x == cmp.x)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool operator<(const POINT& cmp) const
        {
            return !(*this > cmp || *this == cmp);
        }
    };

   public:
    laneft();
    ~laneft();

    double get_feature(unsigned char* src, int srcWidth, int srcHeight);

    void set_find_point_rule(int maskSize, int threshold);
    void set_find_line_rule(int maxDist, int threshold);
    void set_line_height_filter(int threshold);

   protected:
    std::vector<struct POINT> ptList;
    std::vector<std::vector<struct POINT> > hashSpace;
    std::vector<std::vector<struct POINT> > lineHandle;

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
    double line_to_feature(int imgWidth);

    int sum_mask(unsigned char* src, int srcWidth, int srcHeight, int row,
                 int col);

    int hashRowStep;
    int hashColStep;

    int maskSize;
    int pointTh;
    int maxDist;
    int lineTh;
    int lineHeightTh;
};

#endif  // LANEFT_HPP_INCLUDED
