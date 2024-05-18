#include "grahamscan.h"
#include <algorithm>
#include <cmath>
#include <deque>
#include <set>

GrahamScan::GrahamScan(const QVector<QPoint>& points)
    : ConvexHull(points), stack(points.size()), referencePoint(points[0]) {
    // Find the point with the lowest y-coordinate (and the leftmost if there are ties)
    for (const auto& point : points) {
        if (point.y() < referencePoint.y() || (point.y() == referencePoint.y() && point.x() < referencePoint.x())) {
            referencePoint = point;
        }
    }
}

QVector<QPoint> GrahamScan::compute() {



    std::sort(points.begin(), points.end());

    std::deque<QPoint> F;

    for (QPoint &T : points)
    {
        while (F.size() >= 2 and orientation(F[F.size()-1],F[F.size()-2],T) < 0)
            F.pop_back();
        F.push_back(T);

        while (F.size() >= 2 and orientation(F[0],F[1],T) > 0)
            F.pop_front();
        F.push_front(T);
    }

    QSet<QPoint> unique(F.begin(), F.end());
    return QVector<QPoint>(unique.begin(), unique.end());
}

QVector<QPoint> GrahamScan::compute_animate() {
    // Similar to compute() but includes animation steps
    QVector<QPoint> sortedPoints = points;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [this](const QPoint& p1, const QPoint& p2) {
        return compare(p1, p2);
    });

    stack.clear();
    stack.push_back(sortedPoints[0]);
    stack.push_back(sortedPoints[1]);

    QVector<QPoint> steps;  // For animation steps

    for (int i = 2; i < sortedPoints.size(); ++i) {
        while (stack.size() > 1 && orientation(stack[stack.size() - 2], stack.back(), sortedPoints[i]) != 2) {
            stack.pop_back();
            steps.push_back(stack.back());  // Add to steps for animation
        }
        stack.push_back(sortedPoints[i]);
        steps.push_back(sortedPoints[i]);  // Add to steps for animation
    }

    return steps;
}

int GrahamScan::orientation(const QPoint& p, const QPoint& q, const QPoint& r) {
    int val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
    if (val == 0) return 0; // Collinear
    return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
}

int GrahamScan::distance(const QPoint& p1, const QPoint& p2) {
    return (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y());
}

bool GrahamScan::compare(const QPoint& p1, const QPoint& p2) const {
    int o = orientation(referencePoint, p1, p2);
    if (o == 0) {
        return distance(referencePoint, p2) >= distance(referencePoint, p1);
    }
    return o == 2;
}
