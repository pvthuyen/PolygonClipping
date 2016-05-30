#pragma once
#include "Vertex.h"
class Intersection
{
public:
	Intersection(const Vertex &s1, const Vertex &s2, const Vertex &c1, const Vertex &c2) {
		x = y = toSource = toClip = 0.0;

		double d = (c2.y - c1.y) * (s2.x - s1.x) - (c2.x - c1.x) * (s2.y - s1.y);

		if (d == 0) {
			return;
		}

		toSource = ((c2.x - c1.x) * (s1.y - c1.y) - (c2.y - c1.y) * (s1.x - c1.x)) / d;

		toClip = ((s2.x - s1.x) * (s1.y - c1.y) - (s2.y - s1.y) * (s1.x - c1.x)) / d;

		if ((0 < toSource && toSource < 1) && (0 < toClip && toClip < 1)) {
			x = s1.x + toSource * (s2.x - s1.x);
			y = s1.y + toSource * (s2.y - s1.y);
		}
	}

	~Intersection() {};

	bool valid() {
		return (0 < toSource && toSource < 1) && (0 < toClip && toClip < 1);
	}
	

	double x, y, toSource, toClip;
};

