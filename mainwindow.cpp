#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIntValidator>
#include "planewidget.h"
#include <QElapsedTimer>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Inside your MainWindow constructor or a suitable initialization function:
    QLineEdit *lineEdit = findChild<QLineEdit*>("pointCount");
    if (lineEdit) {
        lineEdit->setValidator(new QIntValidator(0, 10'000'000, this));  // Adjust range as needed

        // Connect lineEdit for formatting input as numbers with commas
        connect(lineEdit, &QLineEdit::textEdited, [lineEdit](const QString &text) {
            QString numericText = text;
            numericText.remove(',');

            bool ok;
            int number = numericText.toInt(&ok);
            if (ok) {
                QLocale locale(QLocale::English, QLocale::UnitedStates);
                QString formattedNumber = locale.toString(number);

                lineEdit->blockSignals(true);
                lineEdit->setText(formattedNumber);
                lineEdit->blockSignals(false);
            }
        });
    }

    // Point Size Slider
    ui->point_size_slider->setMinimum(1);
    ui->point_size_slider->setMaximum(5);
    ui->point_size_slider->setTickInterval(1);
    ui->point_size_slider->setSingleStep(1);

    // Animation speed slider
    ui->animation_speed_slider->setMinimum(1);
    ui->animation_speed_slider->setMaximum(5);
    ui->animation_speed_slider->setTickInterval(1);
    ui->animation_speed_slider->setSingleStep(1);

    // Animation line width slizer
    ui->animation_line_width_slider->setMinimum(1);
    ui->animation_line_width_slider->setMaximum(5);
    ui->animation_line_width_slider->setTickInterval(1);
    ui->animation_line_width_slider->setSingleStep(1);

    // Inside your MainWindow setup
    PlaneWidget *planeWidget = new PlaneWidget(this);
    ui->pointPlane->setLayout(new QVBoxLayout());  // Ensure the group box has a layout
    ui->pointPlane->layout()->addWidget(planeWidget);  // Add the PlaneWidget to the layout

    this->planeWidget = planeWidget;


    lineEdit = ui->pointCount;
    slider = ui->pointCountSlider;

    initializeValues();

    if (lineEdit && slider) {
        connect(lineEdit, &QLineEdit::textEdited, this, &MainWindow::setSliderValue);
        connect(slider, &QSlider::valueChanged, this, &MainWindow::setLineEditText);
    }



    // In MainWindow constructor or initialization method
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                QColor color;
                switch (index) {
                case 0: color = Qt::white; break;
                case 1: color = Qt::black; break;
                case 2: color = Qt::red; break;
                case 3: color = Qt::blue; break;
                case 4: color = Qt::green; break;
                default: color = Qt::black; break;
                }
                this->planeWidget->setPointColor(color);
            });

    connect(ui->distribution_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                PlaneWidget::Distribution distributionType;
                switch (index) {
                case 0: distributionType = PlaneWidget::Distribution::Uniform; break;
                case 1: distributionType = PlaneWidget::Distribution::Gaussian; break;
                default: distributionType = PlaneWidget::Distribution::Uniform; break;
                }
                this->planeWidget->setDistribution(distributionType);
            });



    connect(ui->point_style_dot_radio, &QRadioButton::toggled, this, &MainWindow::updatePointStyle);
    connect(ui->point_style_ellipsis, &QRadioButton::toggled, this, &MainWindow::updatePointStyle);
    connect(ui->gen_visible_checkbox, &QCheckBox::toggled, this, &MainWindow::handleVisibilityChange);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleVisibilityChange(bool checked) {
    planeWidget->setOnlyVisible(checked);
}

void MainWindow::setSliderValue(const QString &text) {
    QString numericText = text;
    numericText.remove(',');  // Remove commas before conversion

    bool ok;
    int value = numericText.toInt(&ok);
    if (ok) {
        slider->setValue(value);
    }
}


void MainWindow::setLineEditText(int value) {
    if (lineEdit) {
        QLocale locale(QLocale::English, QLocale::UnitedStates); // ensure consistency in formatting
        QString formattedValue = locale.toString(value);

        ui->pointCount->blockSignals(true);
        ui->pointCount->setText(formattedValue);
        ui->pointCount->blockSignals(false);
    }
}
void MainWindow::initializeValues() {
    qDebug() << "Initializing values...";
    if (!slider) {
        qDebug() << "Slider is null!";
        return;
    }
    if (!lineEdit) {
        qDebug() << "LineEdit is null!";
        return;
    }

    int initialValue = 1000;
    ui->pointCountSlider->setMinimum(0);

    ui->pointCountSlider->setMaximum(10'000'000); // Adjusted to a more reasonable value

    ui->pointCountSlider->setValue(initialValue);

    qDebug() << "Initialization complete.";
}

void MainWindow::updatePointStyle() {
    if (ui->point_style_dot_radio->isChecked()) {
        planeWidget->setPointStyle(PlaneWidget::PointStyle::Dot);
    } else if (ui->point_style_ellipsis->isChecked()) {
        planeWidget->setPointStyle(PlaneWidget::PointStyle::Ellipsis);
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    // Generate button
    QString numericText = ui->pointCount->text();
    numericText.remove(',');  // Remove commas before conversion

    bool ok;
    int pointCount = numericText.toInt(&ok);
    planeWidget->generateRandomPoints(pointCount);
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->radioButton->isChecked()) {
        this->planeWidget->setAlgorithm(PlaneWidget::Algorithm::G);
    } else if (ui->radioButton_2->isChecked()) {
        this->planeWidget->setAlgorithm(PlaneWidget::Algorithm::J);
    } else if (ui->radioButton_3->isChecked()) {
        this->planeWidget->setAlgorithm(PlaneWidget::Algorithm::M);
    } else if (ui->radioButton_4->isChecked()) {
        this->planeWidget->setAlgorithm(PlaneWidget::Algorithm::Q);
    }

    this->planeWidget->computeConvexHull();

    QLabel* runtimeLabel = findChild<QLabel*>("runtime_label");
    runtimeLabel->setText(QString("Runtime: %1 ms").arg(this->planeWidget->getRuntime()));

}


void MainWindow::on_checkBox_stateChanged(int arg1)
{

}



void MainWindow::on_pushButton_3_clicked()
{
    this->planeWidget->runTests();
}

