#ifndef GRAHAMSCAN_H
#define GRAHAMSCAN_H

#include <QVector>
#include <QPoint>

#include "convexhull.h"

class GrahamScan : public ConvexHull {
public:
    GrahamScan(const QVector<QPoint>& points);
    QVector<QPoint> compute() override;
    QVector<QPoint> compute_animate() override;

private:
    QVector<QPoint> stack;
    static int orientation(const QPoint& p, const QPoint& q, const QPoint& r);
    static int distance(const QPoint& p1, const QPoint& p2);
    QPoint referencePoint; // Reference point for sorting
    bool compare(const QPoint& p1, const QPoint& p2) const;
};

#endif // GRAHAMSCAN_H
