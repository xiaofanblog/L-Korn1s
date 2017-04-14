#pragma once
#include "Template.h"
#include "cmath"
#include <map>
#define PI 3.14159265f


inline bool GetSegmentSegmentIntersections(
	Vec2 lineSegment1Start,
	Vec2 lineSegment1End,
	Vec2 lineSegment2Start,
	Vec2 lineSegment2End,
	Vec2& intersectpoint)
{
	float deltaACy = lineSegment1Start.y - lineSegment2Start.y;
	float deltaDCx = lineSegment2End.x - lineSegment2Start.x;
	float deltaACx = lineSegment1Start.x - lineSegment2Start.x;
	float deltaDCy = lineSegment2End.y - lineSegment2Start.y;
	float deltaBAx = lineSegment1End.x - lineSegment1Start.x;
	float deltaBAy = lineSegment1End.y - lineSegment1Start.y;

	float denominator = deltaBAx * deltaDCy - deltaBAy * deltaDCx;
	float numerator = deltaACy * deltaDCx - deltaACx * deltaDCy;

	if (abs(denominator) < std::numeric_limits<float>::epsilon())
	{
		if (abs(numerator) < std::numeric_limits<float>::epsilon())
		{
			// collinear. Potentially infinite intersection points.
			// Check and return one of them.
			if (lineSegment1Start.x >= lineSegment2Start.x && lineSegment1Start.x <= lineSegment2End.x)
			{
				intersectpoint = lineSegment1Start;
				return true;
			}
			if (lineSegment2Start.x >= lineSegment1Start.x && lineSegment2Start.x <= lineSegment1End.x)
			{
				intersectpoint = lineSegment2Start;
				return true;
			}
			return false;
		}
		// parallel
		return false;
	}

	float r = numerator / denominator;
	if (r < 0 || r > 1)
	{
		return false;
	}

	float s = (deltaACy * deltaBAx - deltaACx * deltaBAy) / denominator;
	if (s < 0 || s > 1)
	{
		return false;
	}
	intersectpoint.x = lineSegment1Start.x + r * deltaBAx;
	intersectpoint.y = lineSegment1Start.y + r * deltaBAy;
	return true;
}
inline bool GetSegmentSegmentIntersections(
	Vec3 lineSegment1Start,
	Vec3 lineSegment1End,
	Vec3 lineSegment2Start,
	Vec3 lineSegment2End)
{
	Vec2 out;
	return GetSegmentSegmentIntersections(ToVec2(lineSegment1Start), ToVec2(lineSegment1End), ToVec2(lineSegment2Start), ToVec2(lineSegment2End), out);
}