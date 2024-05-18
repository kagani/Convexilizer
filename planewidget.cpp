#include "PlaneWidget.h"
#include <random>
#include <algorithm>
#include <stack>
#include "grahamscan.h"
#include "quickhull.h"
#include "jarvismarch.h"
#include "mergehull.h"
#include <random>
#include <QRectF>
#include <QPoint>
#include <QVector>

PlaneWidget::PlaneWidget(QWidget *parent) : QWidget(parent), m_currentIndex(0), m_animationActive(false) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &PlaneWidget::updateHullAnimation);
}

void PlaneWidget::startHullAnimation() {
    m_hullPoints.clear();
    m_currentIndex = 0;
    m_animationActive = true;
    m_animationTimer->start(100);  // Adjust the interval to control animation speed
}

void PlaneWidget::updateHullAnimation() {
    if (m_currentIndex >= m_points.size()) {
        m_animationTimer->stop();
        m_animationActive = false;
        return;
    }

    // Animation logic here, e.g., testing edges, selecting hull points
    // Update m_hullPoints and m_currentEdge as needed

    m_currentIndex++;
    update();  // Trigger a repaint to show the current state
}

void PlaneWidget::setDistribution(Distribution d) {
    switch (d) {
    case Distribution::Uniform: this->dist = Distribution::Uniform; break;
    case Distribution::Gaussian: this->dist = Distribution::Gaussian; break;
    default:
        this->dist = Distribution::Uniform; break;
    }
}



void PlaneWidget::generateRandomPoints(int pointCount) {
    // Reset hull
    m_hullPoints.clear();
    m_points.clear();
    QRectF area = onlyVisibleArea ? visibleArea() : QRectF(0, 0, width() * 10, height() * 10);

    std::random_device rd;
    std::mt19937 gen(rd());

    if (this->dist == Distribution::Uniform) {
        std::uniform_int_distribution<> distribX(area.left(), area.right());
        std::uniform_int_distribution<> distribY(area.top(), area.bottom());
        for (int i = 0; i < pointCount; ++i) {
            m_points.push_back(QPoint(distribX(gen), distribY(gen)));
        }
    } else if (this->dist == Distribution::Gaussian) {
        double centerX = area.center().x();
        double centerY = area.center().y();
        double rangeX = area.width() / 6; // 3 sigma should cover 99.7% thus range/6 gives good spread
        double rangeY = area.height() / 6;
        std::normal_distribution<> distribX(centerX, rangeX);
        std::normal_distribution<> distribY(centerY, rangeY);
        for (int i = 0; i < pointCount; ++i) {
            // Round the coordinates since QPoint expects integer values
            m_points.push_back(QPoint(static_cast<int>(distribX(gen)), static_cast<int>(distribY(gen))));
        }
    } else {
        // Default to uniform distribution if an invalid type is provided
        std::uniform_int_distribution<> distribX(area.left(), area.right());
        std::uniform_int_distribution<> distribY(area.top(), area.bottom());
        for (int i = 0; i < pointCount; ++i) {
            m_points.push_back(QPoint(distribX(gen), distribY(gen)));
        }
    }

    // update();  // Update the widget to redraw the points
}

QSize PlaneWidget::sizeHint() const {
    // Calculate the preferred size based on the zoom factor
    return QSize(baseWidth * zoomFactor, baseHeight * zoomFactor);
}

void PlaneWidget::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.1;
    if (event->angleDelta().y() > 0) {
        zoomFactor *= scaleFactor;
    } else {
        zoomFactor /= scaleFactor;
    }
    zoomFactor = qMax(0.1, zoomFactor); // Prevent excessive zoom out

    updateGeometry(); // Notify layout system to recheck the sizeHint
    update();         // Redraw the widget
}


double PlaneWidget::getZoomFactor() const {
    return zoomFactor;
}

void PlaneWidget::setPointColor(QColor color) {
    this->pointColor = color;
}

void PlaneWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragActive = false;  // Reset drag activity flag
        lastMousePosition = event->pos();
    }
}


void PlaneWidget::addPoint(const QPoint& point) {
    m_points.append(point);
}

void PlaneWidget::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        QPoint mouseDelta = event->pos() - lastMousePosition;
        if (mouseDelta.manhattanLength() > 5) {  // Threshold for drag start
            translateX += mouseDelta.x();
            translateY += mouseDelta.y();
            update();  // Update the widget to reflect the translation
            lastMousePosition = event->pos();
            dragActive = true;  // Indicate that actual dragging has occurred
        }
    }
}

void PlaneWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (!dragActive) {  // It was a click, not a drag
            QPointF clickPos = event->pos();
            double invZoom = 1.0 / zoomFactor;
            int x = static_cast<int>((clickPos.x() - translateX) * invZoom);
            int y = static_cast<int>((clickPos.y() - translateY) * invZoom);
            addPoint(QPoint(x, y));  // Add the point where the click occurred
            update();  // Redraw to show the new point
        }
        dragging = false;
        dragActive = false;  // Reset drag activity flag
    }
}

void PlaneWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(translateX, translateY);
    painter.scale(zoomFactor, zoomFactor);

    // Draw all points
    painter.setPen(this->pointColor);
    for (const QPoint &point : m_points) {
        switch (currentStyle) {
        case PointStyle::Dot:
            painter.drawPoint(point);
            break;
        case PointStyle::Ellipsis:
            painter.drawEllipse(point, 3, 3);
            break;
        }
    }

    // Draw the current edge being tested
    painter.setPen(Qt::blue);
    for (int i = 0; i < m_currentEdge.size() - 1; i++) {
        painter.drawLine(m_currentEdge[i], m_currentEdge[i + 1]);
    }

    // Draw the convex hull
    painter.setPen(Qt::red);
    if (!m_hullPoints.isEmpty()) {
        for (int i = 0; i < m_hullPoints.size(); i++) {
            int nextIndex = (i + 1) % m_hullPoints.size();
            painter.drawLine(m_hullPoints[i], m_hullPoints[nextIndex]);
        }
    }
}


void PlaneWidget::setOnlyVisible(bool visible) {
    onlyVisibleArea = visible;
}

void PlaneWidget::setPointStyle(PointStyle style) {
    currentStyle = style;
}

QRectF PlaneWidget::visibleArea() const {
    double visibleWidth = width() / zoomFactor;
    double visibleHeight = height() / zoomFactor;
    double left = -translateX / zoomFactor;
    double top = -translateY / zoomFactor;
    return QRectF(left, top, visibleWidth, visibleHeight);
}



// Helper function to find the orientation of the ordered triplet (p, q, r).
// The function returns following values
// 0 -> p, q and r are collinear
// 1 -> Clockwise
// 2 -> Counterclockwise
int orientation(const QPoint &p, const QPoint &q, const QPoint &r) {
    int val = (q.y() - p.y()) * (r.x() - q.x()) -
              (q.x() - p.x()) * (r.y() - q.y());
    if (val == 0) return 0;  // collinear
    return (val > 0) ? 1: 2; // clock or counterclock wise
}

void PlaneWidget::setAlgorithm(Algorithm algorithm) {
    switch (algorithm) {
    case Algorithm::G: this->algorithm = new GrahamScan(this->m_points); break;
    case Algorithm::J: this->algorithm = new JarvisMarch(this->m_points); break;
    case Algorithm::Q: this->algorithm = new QuickHull(this->m_points); break;
    case Algorithm::M: this->algorithm = new MergeHull(this->m_points); break;
    default:
        break;
    }
}

qint64 PlaneWidget::getRuntime() {
    return this->runtime;
}

// Function to implement Graham's scan convex hull algorithm
void PlaneWidget::computeConvexHull() {
    QElapsedTimer timer;
    timer.start();  // Start the timer just before the computation
    m_hullPoints = this->algorithm->compute();
    this->runtime = timer.elapsed();  // Get the elapsed time in milliseconds
    update();
}

void PlaneWidget::runTests() {
    std::vector<Distribution> distributions{Distribution::Uniform, Distribution::Gaussian};
    std::vector<Algorithm> algorithms{Algorithm::G, Algorithm::J, Algorithm::M, Algorithm::Q};
    std::vector<int> pointCounts{1'000'000};
    setOnlyVisible(false);
    for (auto dist : distributions) {
        qDebug() << "Testing " << (dist == Distribution::Uniform ? "Uniform" : "Gaussian");
        setDistribution(dist);
            for (auto pointCount : pointCounts) {
                for (int step = 6; step < 9; step++) {
                    generateRandomPoints(pointCount + pointCount * step);
                    qDebug() << "Points: " << pointCount + pointCount * step;
                    for (auto algo : algorithms) {
                        setAlgorithm(algo);
                        std::string s;
                        switch (algo) {
                        case Algorithm::G: s = "Graham Scan"; break;
                        case Algorithm::J: s = "Jarvis March"; break;
                        case Algorithm::Q: s = "QuickHull"; break;
                        case Algorithm::M: s = "Merge Hull"; break;
                        default:
                            break;
                        }
                        auto start = std::chrono::high_resolution_clock::now();
                        m_hullPoints = algorithm->compute();
                        auto end = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                        qDebug() << "Algorithm: " << s << " Runtime: " << duration << " ms";
                        m_hullPoints.clear();
                }

            }
        }
    }
}

