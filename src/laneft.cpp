#include <algorithm>
#include <functional>

#include "laneft.hpp"

#define HASH_SPACE 10

#define DEFAULT_MASK_SIZE 1
#define DEFAULT_POINT_TH 128
#define DEFAULT_MAX_DIST 9
#define DEFAULT_LINE_TH 0
#define DEFAULT_LINE_H_TH 0

using namespace std;

bool laneft::POINT::operator>(const POINT& cmp) const
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

bool laneft::POINT::operator==(const POINT& cmp) const
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

bool laneft::POINT::operator<(const POINT& cmp) const
{
    return !(*this > cmp || *this == cmp);
}

laneft::laneft() { this->laneft_init(laneft::LANE_TYPE::LANE); }
laneft::laneft(enum LANE_TYPE laneType) { this->laneft_init(laneType); }

void laneft::set_lane_type(enum laneft::LANE_TYPE laneType)
{
    this->laneType = laneType;
}

void laneft::laneft_init(enum laneft::LANE_TYPE laneType)
{
    // Clear point list
    this->ptList.clear();

    // Create hash space
    this->hashSpace.resize(HASH_SPACE * HASH_SPACE);
    for (int i = 0; i < HASH_SPACE * HASH_SPACE; i++)
    {
        this->hashSpace.at(i).clear();
    }

    // Clear line handle
    this->lineHandle.clear();

    // Set default setting
    this->laneType = laneType;
    this->maskSize = DEFAULT_MASK_SIZE;
    this->pointTh = DEFAULT_POINT_TH;
    this->maxDist = DEFAULT_MAX_DIST;
    this->lineTh = DEFAULT_LINE_TH;
    this->lineHeightTh = DEFAULT_LINE_H_TH;
}

laneft::~laneft()
{
    this->ptList.clear();
    this->del_hash_space();
    this->del_line_handle();
}

void laneft::set_find_point_rule(int maskSize, int threshold)
{
    this->maskSize = this->maskSize;
    this->pointTh = threshold;
}

void laneft::set_find_line_rule(int maxDist, int threshold)
{
    this->maxDist = this->maxDist;
    this->lineTh = threshold;
}

void laneft::set_line_height_filter(int threshold)
{
    this->lineHeightTh = threshold;
}

void laneft::clear_hash_space()
{
    // Clear hash space
    for (int i = 0; i < HASH_SPACE * HASH_SPACE; i++)
    {
        this->hashSpace.at(i).clear();
    }
}

void laneft::del_hash_space()
{
    this->clear_hash_space();
    this->hashSpace.clear();
}

void laneft::clear_line_handle()
{
    // Clear line handle
    for (unsigned int i = 0; i < this->lineHandle.size(); i++)
    {
        this->lineHandle.at(i).clear();
    }
}

void laneft::del_line_handle()
{
    this->clear_line_handle();
    this->lineHandle.clear();
}

int laneft::sum_mask(unsigned char* src, int srcWidth, int srcHeight, int row,
                     int col)
{
    int i, j;
    int tmpRow, tmpCol;
    int result = 0;

    for (i = 0; i < this->maskSize; i++)
    {
        tmpRow = i - this->maskSize / 2 + row;
        if (tmpRow < 0 || tmpRow >= srcHeight)
        {
            return 0;
        }

        for (j = 0; j < this->maskSize; j++)
        {
            tmpCol = j - this->maskSize / 2 + col;
            if (tmpCol < 0 || tmpCol >= srcWidth)
            {
                return 0;
            }

            result += src[tmpRow * srcWidth + tmpCol];
        }
    }

    return result;
}

void laneft::find_point_list(unsigned char* src, int srcWidth, int srcHeight)
{
    int i, j;
    int tmpTh;
    struct POINT tmpPoint;

    tmpTh = this->maskSize * this->maskSize * this->pointTh;

    // Clear point list
    this->ptList.clear();

    for (i = 0; i < srcHeight; i++)
    {
        for (j = 0; j < srcWidth; j++)
        {
            if (this->sum_mask(src, srcWidth, srcHeight, i, j) >= tmpTh)
            {
                // Set temp point
                tmpPoint.x = j;
                tmpPoint.y = i;

                // Insert element
                this->ptList.push_back(tmpPoint);
            }
        }
    }
}

void laneft::generate_line(std::vector<struct POINT>& line,
                           struct POINT startPoint)
{
    unsigned int i, j, k;
    int hashRow, hashCol;
    int tmpRow, tmpCol;

    struct POINT tmpPoint;

    // Find current hash space
    hashRow = startPoint.y / this->hashRowStep;
    hashCol = startPoint.x / this->hashColStep;

    // Search points around current hash space
    for (i = 0; i < 3; i++)
    {
        tmpRow = i - 1 + hashRow;
        if (tmpRow < 0 || tmpRow >= HASH_SPACE)
        {
            continue;
        }

        for (j = 0; j < 3; j++)
        {
            tmpCol = j - 1 + hashCol;
            if (tmpCol < 0 || tmpCol >= HASH_SPACE)
            {
                continue;
            }

            for (k = 0;
                 k < this->hashSpace.at(tmpRow * HASH_SPACE + tmpCol).size();
                 k++)
            {
                tmpPoint =
                    this->hashSpace.at(tmpRow * HASH_SPACE + tmpCol).at(k);
                if ((tmpPoint.x - startPoint.x) * (tmpPoint.x - startPoint.x) +
                        (tmpPoint.y - startPoint.y) *
                            (tmpPoint.y - startPoint.y) <=
                    this->maxDist * this->maxDist)
                {
                    line.push_back(tmpPoint);
                    this->hashSpace.at(tmpRow * HASH_SPACE + tmpCol)
                        .erase(this->hashSpace.at(tmpRow * HASH_SPACE + tmpCol)
                                   .begin() +
                               k);
                    k--;

                    this->generate_line(line, tmpPoint);
                }
            }
        }
    }
}

void laneft::find_line()
{
    unsigned int i, j;
    int hashRow, hashCol;

    std::vector<struct POINT> tmpLine;

    // Create hash space
    this->hashSpace.resize(HASH_SPACE * HASH_SPACE);
    for (i = 0; i < HASH_SPACE * HASH_SPACE; i++)
    {
        this->hashSpace.at(i).clear();
    }

    // Insert point to hash space
    for (i = 0; i < this->ptList.size(); i++)
    {
        hashRow = this->ptList.at(i).y / this->hashRowStep;
        hashCol = this->ptList.at(i).x / this->hashColStep;

        this->hashSpace.at(hashRow * HASH_SPACE + hashCol)
            .push_back(this->ptList.at(i));
    }

    // Clear line handle
    this->lineHandle.clear();

    // Generate line
    for (i = 0; i < HASH_SPACE * HASH_SPACE; i++)
    {
        // Clear line
        tmpLine.clear();

        for (j = 0; j < this->hashSpace.at(i).size(); j++)
        {
            this->generate_line(tmpLine, this->hashSpace.at(i).at(j));
        }

        // Insert temp line to line handle
        if (!tmpLine.empty() && (int)tmpLine.size() > this->lineTh)
        {
            this->lineHandle.push_back(tmpLine);
        }
    }
}

// Clean line handle, reserve the closest point to side only.
void laneft::clean_line(int imgWidth)
{
    for (unsigned int i = 0; i < this->lineHandle.size(); i++)
    {
        // Sort line handle
        sort(this->lineHandle.at(i).begin(), this->lineHandle.at(i).end(),
             greater<POINT>());

        // Clean point
        struct POINT tmpPoint = this->lineHandle.at(i).at(0);
        if (tmpPoint.x < imgWidth / 2)
        {
            for (unsigned int j = 1; j < this->lineHandle.at(i).size(); j++)
            {
                if (this->lineHandle.at(i).at(j).y ==
                    this->lineHandle.at(i).at(j - 1).y)
                {
                    if (this->lineHandle.at(i).at(j).x <=
                        this->lineHandle.at(i).at(j - 1).x)
                    {
                        this->lineHandle.at(i).erase(
                            this->lineHandle.at(i).begin() + j);
                        j--;
                    }
                }
            }
        }
        else
        {
            for (unsigned int j = 0; j < this->lineHandle.at(i).size() - 1; j++)
            {
                if (this->lineHandle.at(i).at(j).y ==
                    this->lineHandle.at(i).at(j + 1).y)
                {
                    if (this->lineHandle.at(i).at(j).x >=
                        this->lineHandle.at(i).at(j + 1).x)
                    {
                        this->lineHandle.at(i).erase(
                            this->lineHandle.at(i).begin() + j);
                        j--;
                    }
                }
            }
        }
    }
}

double laneft::lane_to_feature(int imgWidth)
{
    unsigned int i, j;

    struct POINT tmpPoint;
    struct POINT avgPoint;

    int lCount, rCount;
    double lResult, rResult;
    double result;

    lCount = 0;
    rCount = 0;
    lResult = 0;
    rResult = 0;
    for (i = 0; i < this->lineHandle.size(); i++)
    {
        tmpPoint.x = this->lineHandle.at(i).at(0).x;
        tmpPoint.y = this->lineHandle.at(i).at(0).y;
        avgPoint.x = this->lineHandle.at(i).at(0).x;
        avgPoint.y = this->lineHandle.at(i).at(0).y;
        for (j = 1; j < this->lineHandle.at(i).size(); j++)
        {
            /*
            if(tmpPoint.y < this->lineHandle.at(i).at(j).y)
            {
                tmpPoint.y = this->lineHandle.at(i).at(j).y;
                tmpPoint.x = this->lineHandle.at(i).at(j).x;
            }
            */

            avgPoint.x += this->lineHandle.at(i).at(j).x;
            avgPoint.y += this->lineHandle.at(i).at(j).y;
        }

        avgPoint.x = (double)avgPoint.x / (double)this->lineHandle.at(i).size();
        avgPoint.y = (double)avgPoint.y / (double)this->lineHandle.at(i).size();

        if (tmpPoint.x < imgWidth / 2)
        {
            lCount++;
            lResult += 0 - avgPoint.x;
        }
        else
        {
            rCount++;
            rResult += imgWidth - avgPoint.x;
        }
    }

    result = 0;
    if (lCount > 0)
    {
        result += lResult / (double)lCount;
    }
    if (rCount > 0)
    {
        result += rResult / (double)rCount;
    }

    return result;
}

double laneft::line_to_feature(int imgWidth)
{
    unsigned int i, j;

    struct POINT avgPoint;

    int count = 0;
    double result = 0;
    double target = (double)imgWidth / 2.0;
    for (i = 0; i < this->lineHandle.size(); i++)
    {
        avgPoint.x = 0;
        // avgPoint.y = 0;

        for (j = 0; j < this->lineHandle.at(i).size(); j++)
        {
            avgPoint.x += this->lineHandle.at(i).at(j).x;
            // avgPoint.y += this->lineHandle.at(i).at(j).y;
        }

        avgPoint.x = (double)avgPoint.x / (double)this->lineHandle.at(i).size();
        // avgPoint.y = (double)avgPoint.y /
        // (double)this->lineHandle.at(i).size();

        result += target - avgPoint.x;
        count++;
    }

    if (count)
    {
        return result / (double)count;
    }
    else
    {
        return 0;
    }
}

double laneft::get_feature(unsigned char* src, int srcWidth, int srcHeight)
{
    // Find hash step
    this->hashRowStep = (srcHeight % HASH_SPACE == 0)
                            ? srcHeight / HASH_SPACE
                            : srcHeight / HASH_SPACE + 1;
    this->hashColStep = (srcWidth % HASH_SPACE == 0)
                            ? srcWidth / HASH_SPACE
                            : srcWidth / HASH_SPACE + 1;

    // Clear vectors
    this->clear_hash_space();
    this->clear_line_handle();

    // Find feature
    this->find_point_list(src, srcWidth, srcHeight);
    this->find_line();

    // Processing line height filter
    if (this->lineHeightTh > 0)
    {
        this->line_height_filter();
    }

    // Clean line
    this->clean_line(srcWidth);

    // Get feature
    if (this->laneType == laneft::LANE_TYPE::LANE)
    {
        return this->lane_to_feature(srcWidth);
    }
    else
    {
        return this->line_to_feature(srcWidth);
    }
}

void laneft::line_height_filter()
{
    unsigned int i, j;
    int yMax, yMin;

    for (i = 0; i < this->lineHandle.size(); i++)
    {
        yMax = this->lineHandle.at(i).at(0).y;
        yMin = this->lineHandle.at(i).at(0).y;

        for (j = 1; j < this->lineHandle.at(i).size(); j++)
        {
            if (yMax < this->lineHandle.at(i).at(j).y)
                yMax = this->lineHandle.at(i).at(j).y;
            if (yMin > this->lineHandle.at(i).at(j).y)
                yMin = this->lineHandle.at(i).at(j).y;
        }

        if (yMax - yMin < this->lineHeightTh)
        {
            this->lineHandle.erase(this->lineHandle.begin() + i);
            i--;
        }
    }
}
