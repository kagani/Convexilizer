#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <QVector>
#include <QPoint>
#include <QDebug>
#include <functional>


// Animation brush
using LineBrush = std::function<void(const QPoint&, const QPoint&)>;

class ConvexHull {
public:
    ConvexHull(const QVector<QPoint> &points, const LineBrush hullBrush = nullptr, const LineBrush &stepBrush = nullptr, const LineBrush &clearBrush = nullptr) : points(points), hull_brush(hullBrush), step_brush(stepBrush), clear_brush(clearBrush) {}
    virtual ~ConvexHull() {}

    // Compute the convex hull
    virtual QVector<QPoint> compute() = 0;
    // Same as compute() but will animate.
    // I didn't want to include if statements in compute() for performance.
    virtual QVector<QPoint> compute_animate() = 0;

    QVector<QPoint> get_hull() const {
        return current_hull;
    }

    void setHullBrush(LineBrush &brush) {
        this->hull_brush = brush;
    }

    void setStepBrush(LineBrush &brush) {
        this->step_brush = brush;
    }

    void setClearBrush(LineBrush &brush) {
        this->clear_brush = brush;
    }

    void clearTrialSteps() {
        if (clear_brush) {
            // Erase each line segment formed by consecutive points in hull_step
            for (int i = 0; i < hull_step.size() - 1; i++) {
                step_brush(hull_step[i], hull_step[i+1]); // Assuming step_brush can also "erase" lines
            }
        } else {
            qDebug() << "WARNING: Clear brush was not provided!";
        }
        hull_step.clear(); // Clear the stored step points
    }

protected:
    QVector<QPoint> points;
    QVector<QPoint> current_hull;
    QVector<QPoint> hull_step; // Trial step points, this is used to clear those lines
    LineBrush hull_brush; // Final hull line
    LineBrush step_brush; // Trial step line
    LineBrush clear_brush; // Brush for removing drawn lines

};

#endif // CONVEXHULL_H
