#ifndef JARVISMARCH_H
#define JARVISMARCH_H

#include <QVector>
#include <QPoint>
#include <QDebug>

#include "convexhull.h"

class JarvisMarch : public ConvexHull
{
public:
    JarvisMarch(const QVector<QPoint>& points)
        : ConvexHull(points) {}

    QVector<QPoint> compute_animate() override {}

    // Function to compute the convex hull
    QVector<QPoint> compute() override
    {
        int n = points.size();
        QVector<QPoint> hull;

        if (n < 3)
            return hull; // Empty hull for less than 3 points

        // Find the leftmost point
        int l = 0;
        for (int i = 1; i < n; i++)
            if (points[i].x() < points[l].x())
                l = i;

        // Start from leftmost point, keep moving counterclockwise
        int p = l, q;
        do {
            // Add current point to result
            hull.append(points[p]);

            // Search for a point 'q' such that orientation(p, q, x) is counterclockwise for all points 'x'.
            q = (p + 1) % n;
            for (int i = 0; i < n; i++) {
                if (orientation(points[p], points[i], points[q]) == 2)
                    q = i;
            }

            // Now q is the most counterclockwise with respect to p
            p = q;

        } while (p != l); // While we don't come to first point

        return hull;
    }

private:
    // To find orientation of ordered triplet (p, q, r).
    int orientation(const QPoint& p, const QPoint& q, const QPoint& r) const
    {
        int val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());

        if (val == 0) return 0;  // collinear
        return (val > 0) ? 1 : 2; // clock or counterclock wise
    }
};

#endif // JARVISMARCH_H
