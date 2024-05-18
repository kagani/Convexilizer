#ifndef MERGEHULL_H
#define MERGEHULL_H

#include <QVector>
#include <QPoint>
#include <algorithm>
#include <QDebug>
#include "convexhull.h"

class MergeHull : public ConvexHull {
public:
    MergeHull(const QVector<QPoint>& points)
        : ConvexHull(points) {}

    QVector<QPoint> compute() override {
        return _compute(0, points.size(), 0);
    }

    QVector<QPoint> compute_animate() override {}

private:
    QVector<QPoint> _compute(int start, int end, int direction = 0) {
        QVector<QPoint>& S = points;

        // Base case
        int size = end - start;
        if (size <= 3) {
            QVector<QPoint> baseHull;
            for (int i = start; i < end; ++i) {
                baseHull.append(S[i]);
            }
            return grahamScan(baseHull); // Ensure the base case returns a proper hull
        }

        int newDir = direction == 0 ? 1 : 0;   // Change direction

        // Sort
        if (direction == 1) {
            std::sort(S.begin() + start, S.begin() + end, [](const QPoint& a, const QPoint& b) {
                if (a.x() != b.x()) {
                    return a.x() < b.x(); // Sort by x-coordinate first
                } else {
                    return a.y() < b.y(); // If x-coordinates are equal, sort by y-coordinate
                }
            });
        } else {
            std::sort(S.begin() + start, S.begin() + end, [](const QPoint& a, const QPoint& b) {
                if (a.y() != b.y()) {
                    return a.y() < b.y(); // Sort by y-coordinate first
                } else {
                    return a.x() < b.x(); // If y-coordinates are equal, sort by x-coordinate
                }
            });
        }

        // Cut S into half and recursively pass
        int middleIndex = start + size / 2;

        // Recursively pass the two halves
        QVector<QPoint> hullL = _compute(start, middleIndex, newDir);
        QVector<QPoint> hullR = _compute(middleIndex, end, newDir);

        // Merge without animating
        return mergeHulls(hullL, hullR);
    }

    QVector<QPoint> mergeHulls(const QVector<QPoint>& left, const QVector<QPoint>& right) {
        // Simplified merging logic, using Graham's scan to ensure the result is a proper convex hull
        QVector<QPoint> result = left + right;

        return grahamScan(result);
    }

    QVector<QPoint> grahamScan(QVector<QPoint>& points) {
        int n = points.size();
        if (n < 3) return points;

        // Find the bottom-most point
        int minY = points[0].y(), minIdx = 0;
        for (int i = 1; i < n; i++) {
            int y = points[i].y();
            if ((y < minY) || (minY == y && points[i].x() < points[minIdx].x())) {
                minY = points[i].y();
                minIdx = i;
            }
        }

        // Place the bottom-most point at the first position
        std::swap(points[0], points[minIdx]);

        QPoint p0 = points[0];
        // Sort the points based on the polar angle with p0
        std::sort(points.begin() + 1, points.end(), [p0](const QPoint& p1, const QPoint& p2) {
            int o = orientation(p0, p1, p2);
            if (o == 0)
                return (p0.x() - p1.x()) * (p0.x() - p1.x()) + (p0.y() - p1.y()) * (p0.y() - p1.y()) <
                       (p0.x() - p2.x()) * (p0.x() - p2.x()) + (p0.y() - p2.y()) * (p0.y() - p2.y());
            return o == 2;
        });

        // Create an empty stack and push first three points
        QVector<QPoint> hull;
        hull.push_back(points[0]);
        hull.push_back(points[1]);
        hull.push_back(points[2]);

        // Process the remaining points
        for (int i = 3; i < n; i++) {
            while (hull.size() > 1 && orientation(hull[hull.size() - 2], hull.back(), points[i]) != 2) {
                hull.pop_back();
            }
            hull.push_back(points[i]);
        }

        return hull;
    }

    static int orientation(const QPoint& p, const QPoint& q, const QPoint& r) {
        int val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
        if (val == 0) return 0; // collinear
        return (val > 0) ? 1 : 2; // clock or counterclock wise
    }
};

#endif // MERGEHULL_H
