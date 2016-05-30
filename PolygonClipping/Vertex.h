#pragma once
#include <cstdlib>
#include <memory>

typedef std::pair <double, double> Point;

class Vertex
{
public:
	Vertex(double _x = 0, double _y = 0) : x(_x), y(_y) {
		next = prev = corresponding = nullptr;
		distance = 0;
		isEntry = true;
		isIntersection = false;
		isVisited = false;
	};
	static std::shared_ptr<Vertex> createIntersection(double _x, double _y, double _distance) {
		std::shared_ptr<Vertex> vertex(new Vertex(_x, _y));
		vertex->distance = _distance;
		vertex->isIntersection = true;
		vertex->isEntry = false;
		return vertex;
	}
	~Vertex() {};

	void visit() {
		isVisited = true;
		if (corresponding && !corresponding->isVisited)
			corresponding->visit();
	}

	bool operator == (const Vertex &other) {
		return (x == other.x) && (y == other.y);
	}

	std::shared_ptr<Vertex> next, prev, corresponding;
	double distance;
	bool isEntry, isIntersection, isVisited;
	double x, y;
	
};

