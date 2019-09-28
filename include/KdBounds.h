#pragma once

#include "Point.h"

enum BoundsIndices
{
	BI_LOW,
	BI_HIGH,
	BI_MAX
};

struct KdBounds
{
	Point m_low;
	Point m_high;
};