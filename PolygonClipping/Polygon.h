#pragma once
#include <memory>
#include "Vertex.h"
#include "Intersection.h"
#include <vector>

namespace PolygonClipping {
	class Polygon
	{
	public:
		Polygon() {
			first = lastUnprocessed = nullptr;
			vertices = 0;
		};
		Polygon(const std::vector <std::shared_ptr <Vertex>> &points) {
			first = lastUnprocessed = nullptr;
			vertices = 0;

			for (int i = 0; i < points.size(); ++i)
				addVertex(points[i]);
		};
		~Polygon() {};

		void addVertex(std::shared_ptr <Vertex> vertex) {
			if (!first) {
				first = vertex;
				first->next = vertex;
				first->prev = vertex;
			}
			else {
				std::shared_ptr <Vertex> next = first, prev = next->prev;
				next->prev = vertex;
				vertex->next = next;
				vertex->prev = prev;
				prev->next = vertex;
			}
			++vertices;
		}

		void insertVertex(std::shared_ptr <Vertex> vertex, std::shared_ptr <Vertex> start, std::shared_ptr <Vertex> end) {
			std::shared_ptr <Vertex> prev, curr = start;

			while (!(*curr == *end) && curr->distance < vertex->distance) {
				curr = curr->next;
			}

			vertex->next = curr;
			prev = curr->prev;

			vertex->prev = prev;
			prev->next = vertex;
			curr->prev = vertex;

			++vertices;
		}

		std::shared_ptr<Vertex> getNext(std::shared_ptr<Vertex> vertex) {
			std::shared_ptr<Vertex> c = vertex;
			while (c->isIntersection) {
				c = c->next;
			}
			return c;
		}

		std::shared_ptr<Vertex> getFirstIntersect() {
			std::shared_ptr<Vertex> v;
			if (firstIntersect)
				v = firstIntersect;
			else
				v = first;

			do {
				if (v->isIntersection && !v->isVisited) {
					break;
				}

				v = v->next;
			} while (!(*v == *first));

			firstIntersect = v;
			return v;
		}

		bool hasUnprocessed() {
			std::shared_ptr<Vertex> v;
			if (lastUnprocessed)
				v = lastUnprocessed;
			else
				v = first;

			do {
				if (v->isIntersection && !v->isVisited) {
					lastUnprocessed = v;
					return true;
				}

				v = v->next;
			} while (!(*v == *first));

			lastUnprocessed = nullptr;
			return false;
		}

		std::vector <std::shared_ptr <Vertex>> getPoints() {
			std::vector <std::shared_ptr <Vertex>> points;
			std::shared_ptr <Vertex> v = first;

			do {
				points.push_back(std::shared_ptr <Vertex>(new Vertex(v->x, v->y)));
				v = v->next;
			} while (v != first);
			return points;
		}

		bool containInside(std::shared_ptr<Vertex> v) {
			bool oddNodes = false;
			std::shared_ptr<Vertex> vertex = first, next = vertex->next;
			double x = v->x, y = v->y;

			do {
				if ((vertex->y < y && next->y >= y ||
					next->y < y && vertex->y >= y) &&
					(vertex->x <= x || next->x <= x)) {

					oddNodes ^= (vertex->x + (y - vertex->y) /
						(next->y - vertex->y) * (next->x - vertex->x) < x);
				}

				vertex = vertex->next;
				if (vertex->next)
					next = vertex->next;
				else
					next = first;
			} while (!(*vertex == *first));

			return oddNodes;
		}

		std::vector <std::vector <std::shared_ptr <Vertex>>> clip(Polygon clip, bool sourceForwards, bool clipForwards) {
			std::shared_ptr <Vertex> sourceVertex = first, clipVertex = clip.first;
			bool sourceInClip, clipInSource;

			// calculate and mark intersection
			do {
				if (!sourceVertex->isIntersection) {
					do {
						if (!clipVertex->isIntersection) {
							Intersection i(*sourceVertex, *getNext(sourceVertex->next), *clipVertex, *clip.getNext(clipVertex->next));

							if (i.valid()) {
								std::shared_ptr<Vertex> sourceIntersection = Vertex::createIntersection(i.x, i.y, i.toSource);
								std::shared_ptr<Vertex> clipIntersection = Vertex::createIntersection(i.x, i.y, i.toClip);

								sourceIntersection->corresponding = clipIntersection;
								clipIntersection->corresponding = sourceIntersection;

								insertVertex(sourceIntersection, sourceVertex, getNext(sourceVertex->next));
								clip.insertVertex(clipIntersection, clipVertex, clip.getNext(clipVertex->next));
							}
						}

						clipVertex = clipVertex->next;
					} while (!(*clipVertex == *clip.first));
				}
				sourceVertex = sourceVertex->next;
			} while (!(*sourceVertex == *first));

			// phase two - identify entry/exit points
			sourceVertex = first;
			clipVertex = clip.first;

			sourceInClip = clip.containInside(sourceVertex);
			clipInSource = containInside(clipVertex);

			sourceForwards ^= sourceInClip;
			clipForwards ^= clipInSource;

			do {
				if (sourceVertex->isIntersection) {
					sourceVertex->isEntry = sourceForwards;
					sourceForwards = !sourceForwards;
				}
				sourceVertex = sourceVertex->next;
			} while (!(*sourceVertex == *first));

			do {
				if (clipVertex->isIntersection) {
					clipVertex->isEntry = clipForwards;
					clipForwards = !clipForwards;
				}
				clipVertex = clipVertex->next;
			} while (!(*clipVertex == *clip.first));

			// phase three - construct a list of clipped polygons

			std::vector <std::vector <std::shared_ptr <Vertex>>> list;

			while (hasUnprocessed()) {
				std::shared_ptr<Vertex> current = getFirstIntersect();
				Polygon clipped;

				clipped.addVertex(std::shared_ptr<Vertex>(new Vertex(current->x, current->y)));
				do {
					current->visit();
					if (current->isEntry) {
						do {
							current = current->next;
							clipped.addVertex(std::shared_ptr<Vertex>(new Vertex(current->x, current->y)));
						} while (!current->isIntersection);
					}
					else {
						do {
							current = current->prev;
							clipped.addVertex(std::shared_ptr<Vertex>(new Vertex(current->x, current->y)));
						} while (!current->isIntersection);
					}
					current = current->corresponding;
				} while (!current->isVisited);
				list.push_back(clipped.getPoints());


			}

			if (list.size() == 0) {
				if (sourceInClip) {
					list.push_back(getPoints());
				}

				if (clipInSource) {
					list.push_back(clip.getPoints());
				}

			}
			return list;
		}

	private:
		std::shared_ptr <Vertex> first, lastUnprocessed, firstIntersect;
		int vertices;
	};
};

