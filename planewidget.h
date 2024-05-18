// PlaneWidget.h
#ifndef PLANEWIDGET_H
#define PLANEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QWheelEvent>
#include <QTimer>
#include "convexhull.h"

class PlaneWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlaneWidget(QWidget *parent = nullptr);
    void generateRandomPoints(int pointCount);

    void wheelEvent(QWheelEvent *event) override;
    QSize sizeHint() const override;
    double getZoomFactor() const;
    void setPointColor(QColor color);
    double getTranslateX() const;
    double getTranslateY() const;

    enum class PointStyle { Dot, Ellipsis };

    enum class Algorithm { G, J, Q, M};
    void setAlgorithm(Algorithm algorithm);

    enum class Distribution { Uniform, Gaussian};
    void setDistribution(Distribution d);

    void setPointStyle(PointStyle style);
    QRectF visibleArea() const;
    void setOnlyVisible(bool visible);
    bool onlyVisibleArea = true;
    void addPoint(const QPoint& point);

    void computeConvexHull();
    void toggleAnimateConvexHull();
    void startHullAnimation();
    void runTests();


    qint64 getRuntime();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateHullAnimation();

private:
    QVector<QPoint> m_points;
    double zoomFactor = 1.0;
    int baseWidth = 800;   // Base width without zoom
    int baseHeight = 600;  // Base height without zoom
    QColor pointColor = Qt::white;

    double translateX = 0.0;  // X translation of the view
    double translateY = 0.0;  // Y translation of the view

    bool dragging = false;
    QPoint lastMousePosition;

    PointStyle currentStyle = PointStyle::Dot; // Default dot
    bool dragActive = false;

    QVector<QPoint> m_hullPoints;  // Stores the final hull points
    QVector<QPoint> m_currentEdge;  // Temporarily store the current edge being tested

    bool m_animateConvexHull = false;
    int m_currentIndex;  // Index for animation progress
    QTimer *m_animationTimer;
    bool m_animationActive;

    ConvexHull *algorithm;
    qint64 runtime = 0;

    Distribution dist = Distribution::Uniform;
};

#endif
