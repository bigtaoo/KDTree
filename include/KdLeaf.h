#pragma once

#include "KdNode.h"

class KdLeaf : public KdNode
{
public:
	KdLeaf(int size, int offset);
	~KdLeaf();

	void Search(const Point& q, int64_t distance, int64_t& smallDistance, int& smallPointIndex) const override;

private:
	int m_size; // 1 or 0. 
	int m_offset; // the offset indices of this leaf. 
};