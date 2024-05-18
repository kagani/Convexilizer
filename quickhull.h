#ifndef QUICKHULL_H
#define QUICKHULL_H

#include <QVector>
#include <QPoint>
#include <QSet>
#include <QtDebug>
#include <algorithm>

#include "convexhull.h"

class QuickHull : public ConvexHull
{
public:
    QuickHull(const QVector<QPoint>& points)
        : ConvexHull(points) {}

    QVector<QPoint> compute_animate() override {}

    // Compute the convex hull of the previously given points
    QVector<QPoint> compute() override
    {

        int n = points.size();
        if (n < 3)
        {
            qDebug() << "Convex hull not possible";
            return QVector<QPoint>();
        }

        // Find the point with the minimum and maximum x-coordinate
        int min_x = 0, max_x = 0;
        for (int i = 1; i < n; i++)
        {
            if (points[i].x() < points[min_x].x())
                min_x = i;
            if (points[i].x() > points[max_x].x())
                max_x = i;
        }

        // Recursively find convex hull points on both sides of the line joining points[min_x] and points[max_x]
        quickHull(points[min_x], points[max_x], 1);
        quickHull(points[min_x], points[max_x], -1);

        QVector<QPoint> orderedHull(hull.begin(), hull.end());
        orderPoints(orderedHull);
        return orderedHull;
    }

private:
    QSet<QPoint> hull;

    void quickHull(QPoint p1, QPoint p2, int side)
    {
        int ind = -1;
        int max_dist = 0;

        for (int i = 0; i < points.size(); i++)
        {
            int temp = lineDist(p1, p2, points[i]);
            if (findSide(p1, p2, points[i]) == side && temp > max_dist)
            {
                ind = i;
                max_dist = temp;
            }
        }

        if (ind == -1)
        {
            hull.insert(p1);
            hull.insert(p2);
            return;
        }

        quickHull(points[ind], p1, -findSide(points[ind], p1, p2));
        quickHull(points[ind], p2, -findSide(points[ind], p2, p1));
    }

    int findSide(QPoint p1, QPoint p2, QPoint p)
    {
        int val = (p.y() - p1.y()) * (p2.x() - p1.x()) -
                  (p2.y() - p1.y()) * (p.x() - p1.x());

        if (val > 0)
            return 1;
        if (val < 0)
            return -1;
        return 0;
    }

    int lineDist(QPoint p1, QPoint p2, QPoint p)
    {
        return abs((p.y() - p1.y()) * (p2.x() - p1.x()) -
                   (p2.y() - p1.y()) * (p.x() - p1.x()));
    }

    void orderPoints(QVector<QPoint>& points)
    {
        QPoint centroid(0, 0);
        for (const QPoint& p : points)
        {
            centroid += p;
        }
        centroid /= points.size();

        // Sort points based on their angle with the centroid
        std::sort(points.begin(), points.end(), [centroid](const QPoint& a, const QPoint& b) {
            int dx1 = a.x() - centroid.x();
            int dy1 = a.y() - centroid.y();
            int dx2 = b.x() - centroid.x();
            int dy2 = b.y() - centroid.y();
            return atan2(dy1, dx1) < atan2(dy2, dx2);
        });
    }
};

#endif
