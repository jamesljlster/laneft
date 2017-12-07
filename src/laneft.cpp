#include <functional>
#include <algorithm>

#include "laneft.hpp"

#define HASH_SPACE  10

#define DEFAULT_MASK_SIZE   1
#define DEFAULT_POINT_TH    128
#define DEFAULT_MAX_DIST    9
#define DEFAULT_LINE_TH     0
#define DEFAULT_LINE_H_TH   0

using namespace std;

laneft::laneft()
{
	// Clear point list
	ptList.clear();

	// Create hash space
	hashSpace.resize(HASH_SPACE * HASH_SPACE);
	for(int i = 0; i < HASH_SPACE * HASH_SPACE; i++)
	{
		hashSpace.at(i).clear();
	}

	// Clear line handle
	lineHandle.clear();

	// Set default setting
	maskSize = DEFAULT_MASK_SIZE;
	pointTh = DEFAULT_POINT_TH;
	maxDist = DEFAULT_MAX_DIST;
	lineTh = DEFAULT_LINE_TH;
	lineHeightTh = DEFAULT_LINE_H_TH;
}

laneft::~laneft()
{
	ptList.clear();
	del_hash_space();
	del_line_handle();
}

void laneft::set_find_point_rule(int maskSize, int threshold)
{
	maskSize = maskSize;
	pointTh = threshold;
}

void laneft::set_find_line_rule(int maxDist, int threshold)
{
	maxDist = maxDist;
	lineTh = threshold;
}

void laneft::set_line_height_filter(int threshold)
{
	lineHeightTh = threshold;
}

void laneft::clear_hash_space()
{
	// Clear hash space
	for(int i = 0; i < HASH_SPACE * HASH_SPACE; i++)
	{
		hashSpace.at(i).clear();
	}
}

void laneft::del_hash_space()
{
	clear_hash_space();
	hashSpace.clear();
}

void laneft::clear_line_handle()
{
	// Clear line handle
	for(unsigned int i = 0; i < lineHandle.size(); i++)
	{
		lineHandle.at(i).clear();
	}
}

void laneft::del_line_handle()
{
	clear_line_handle();
	lineHandle.clear();
}

int laneft::sum_mask(unsigned char* src, int srcWidth, int srcHeight, int row, int col)
{
	int i, j;
	int tmpRow, tmpCol;
	int result = 0;

	for(i = 0; i < maskSize; i++)
	{
		tmpRow = i - maskSize / 2 + row;
		if(tmpRow < 0 || tmpRow >= srcHeight)
		{
			return 0;
		}

		for(j = 0; j < maskSize; j++)
		{
			tmpCol = j - maskSize / 2 + col;
			if(tmpCol < 0 || tmpCol >= srcWidth)
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

	tmpTh = maskSize * maskSize * pointTh;

	// Clear point list
	ptList.clear();

	for(i = 0; i < srcHeight; i++)
	{
		for(j = 0; j < srcWidth; j++)
		{
			if(sum_mask(src, srcWidth, srcHeight, i, j) >= tmpTh)
			{
				// Set temp point
				tmpPoint.x = j;
				tmpPoint.y = i;

				// Insert element
				ptList.push_back(tmpPoint);
			}
		}
	}
}

void laneft::generate_line(std::vector<struct POINT>& line, struct POINT startPoint)
{
	unsigned int i, j, k;
	int hashRow, hashCol;
	int tmpRow, tmpCol;

	struct POINT tmpPoint;

	// Find current hash space
	hashRow = startPoint.y / hashRowStep;
	hashCol = startPoint.x / hashColStep;

	// Search points around current hash space
	for(i = 0; i < 3; i++)
	{
		tmpRow = i - 1 + hashRow;
		if(tmpRow < 0 || tmpRow >= HASH_SPACE)
		{
			continue;
		}

		for(j = 0; j < 3; j++)
		{
			tmpCol = j - 1 + hashCol;
			if(tmpCol < 0 || tmpCol >= HASH_SPACE)
			{
				continue;
			}

			for(k = 0; k < hashSpace.at(tmpRow * HASH_SPACE + tmpCol).size(); k++)
			{
				tmpPoint = hashSpace.at(tmpRow * HASH_SPACE + tmpCol).at(k);
				if((tmpPoint.x - startPoint.x) * (tmpPoint.x - startPoint.x) + (tmpPoint.y - startPoint.y) * (tmpPoint.y - startPoint.y) <= maxDist * maxDist)
				{
					line.push_back(tmpPoint);
					hashSpace.at(tmpRow * HASH_SPACE + tmpCol).erase(hashSpace.at(tmpRow * HASH_SPACE + tmpCol).begin() + k);
					k--;

					generate_line(line, tmpPoint);
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
	hashSpace.resize(HASH_SPACE * HASH_SPACE);
	for(i = 0; i < HASH_SPACE * HASH_SPACE; i++)
	{
		hashSpace.at(i).clear();
	}

	// Insert point to hash space
	for(i = 0; i < ptList.size(); i++)
	{
		hashRow = ptList.at(i).y / hashRowStep;
		hashCol = ptList.at(i).x / hashColStep;

		hashSpace.at(hashRow * HASH_SPACE + hashCol).push_back(ptList.at(i));
	}

	// Clear line handle
	lineHandle.clear();

	// Generate line
	for(i = 0; i < HASH_SPACE * HASH_SPACE; i++)
	{
		// Clear line
		tmpLine.clear();

		for(j = 0; j < hashSpace.at(i).size(); j++)
		{
			generate_line(tmpLine, hashSpace.at(i).at(j));
		}

		// Insert temp line to line handle
		if(!tmpLine.empty() && (int)tmpLine.size() > lineTh)
		{
			lineHandle.push_back(tmpLine);
		}
	}
}

// Clean line handle, reserve the closest point to side only.
void laneft::clean_line(int imgWidth)
{
	for(unsigned int i = 0; i < lineHandle.size(); i++)
	{
		// Sort line handle
		sort(lineHandle.at(i).begin(), lineHandle.at(i).end(), greater<POINT>());

		// Clean point
		struct POINT tmpPoint = lineHandle.at(i).at(0);
		if(tmpPoint.x < imgWidth / 2)
		{
			for(unsigned int j = 1; j < lineHandle.at(i).size(); j++)
			{
				if(lineHandle.at(i).at(j).y == lineHandle.at(i).at(j - 1).y)
				{
					if(lineHandle.at(i).at(j).x >= lineHandle.at(i).at(j - 1).x)
					{
						lineHandle.at(i).erase(lineHandle.at(i).begin() + j);
						j--;
					}
				}
			}
		}
		else
		{
			for(unsigned int j = 1; j < lineHandle.at(i).size(); j++)
			{
				if(lineHandle.at(i).at(j).y == lineHandle.at(i).at(j - 1).y)
				{
					if(lineHandle.at(i).at(j).x <= lineHandle.at(i).at(j - 1).x)
					{
						lineHandle.at(i).erase(lineHandle.at(i).begin() + j);
						j--;
					}
				}
			}
		}
	}
}

double laneft::line_to_feature(int imgWidth)
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
	for(i = 0; i < lineHandle.size(); i++)
	{
		tmpPoint.x = lineHandle.at(i).at(0).x;
		tmpPoint.y = lineHandle.at(i).at(0).y;
		avgPoint.x = lineHandle.at(i).at(0).x;
		avgPoint.y = lineHandle.at(i).at(0).y;
		for(j = 1; j < lineHandle.at(i).size(); j++)
		{
			/*
			if(tmpPoint.y < lineHandle.at(i).at(j).y)
			{
				tmpPoint.y = lineHandle.at(i).at(j).y;
				tmpPoint.x = lineHandle.at(i).at(j).x;
			}
			*/

			avgPoint.x += lineHandle.at(i).at(j).x;
			avgPoint.y += lineHandle.at(i).at(j).y;
		}

		avgPoint.x = (double)avgPoint.x / (double)lineHandle.at(i).size();
		avgPoint.y = (double)avgPoint.y / (double)lineHandle.at(i).size();

		if(tmpPoint.x < imgWidth / 2)
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
	if(lCount > 0)
	{
		result += lResult / (double)lCount;
	}
	if(rCount > 0)
	{
		result += rResult / (double)rCount;
	}

	return result;
}

double laneft::get_feature(unsigned char* src, int srcWidth, int srcHeight)
{
	// Find hash step
	hashRowStep = (srcHeight % HASH_SPACE == 0) ? srcHeight / HASH_SPACE : srcHeight / HASH_SPACE + 1;
	hashColStep = (srcWidth % HASH_SPACE == 0) ? srcWidth / HASH_SPACE : srcWidth / HASH_SPACE + 1;

	// Clear vectors
	clear_hash_space();
	clear_line_handle();

	// Find feature
	find_point_list(src, srcWidth, srcHeight);
	find_line();

	// Processing line height filter
	if(lineHeightTh > 0)
	{
		line_height_filter();
	}

	// Clean line
	clean_line(srcWidth);

	return line_to_feature(srcWidth);
}

void laneft::line_height_filter()
{
	unsigned int i, j;
	int yMax, yMin;

	for(i = 0; i <lineHandle.size(); i++)
	{
		yMax = lineHandle.at(i).at(0).y;
		yMin = lineHandle.at(i).at(0).y;
		for(j = 1; j < lineHandle.at(i).size(); j++)
		{
			if(yMax < lineHandle.at(i).at(j).y)
				yMax = lineHandle.at(i).at(j).y;
			if(yMin > lineHandle.at(i).at(j).y)
				yMin = lineHandle.at(i).at(j).y;
		}
		if(yMax - yMin < lineHeightTh)
		{
			lineHandle.erase(lineHandle.begin() + i);
			i--;
		}
	}
}
