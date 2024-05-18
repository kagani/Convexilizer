#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QSlider>
#include "planewidget.h"
#include "convexhull.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLineEdit *lineEdit;
    QSlider *slider;
    PlaneWidget *planeWidget;
    void initializeValues(); // Initialization function
    void generateRandomPoints();
    void updateDisplay(const std::vector<QPoint>& points);
    void generateRandomPoints(PlaneWidget *planeWidget);

private slots:
    void setSliderValue(const QString &text);
    void setLineEditText(int value);
    void updatePointStyle();
    void on_pushButton_2_clicked();
    void handleVisibilityChange(bool checked);
    void on_pushButton_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_pushButton_3_clicked();
};
#endif // MAINWINDOW_H
