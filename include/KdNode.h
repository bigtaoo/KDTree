#pragma once

#include <stdint.h>
#include "Point.h"

class KdNode
{
public:
	virtual ~KdNode() {}
	virtual void Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) const = 0;
};