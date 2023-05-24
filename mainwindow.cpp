#include "mainwindow.h"
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    originalImageLabel = new QLabel;
    equalizedImageLabel = new QLabel;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(originalImageLabel);
    layout->addWidget(equalizedImageLabel);

    QPushButton *openButton = new QPushButton("Open Image");
    layout->addWidget(openButton);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openImage);

    QPushButton *saveButton = new QPushButton("Save Image");
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::openImage()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.bmp)");
    if (!imagePath.isEmpty())
    {
        QImage image(imagePath);
        if (image.isNull())
        {
            // Обработка ошибки при загрузке изображения
            return;
        }

        originalImageLabel->setPixmap(QPixmap::fromImage(image));

        equalizedImage = equalizeImage(image);
        equalizedImageLabel->setPixmap(QPixmap::fromImage(equalizedImage));
    }
}

void MainWindow::saveImage()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Save Image", "", "Image Files (*.png *.jpg *.bmp)");
    if (!savePath.isEmpty())
    {
        equalizedImage.save(savePath);
    }
}

QImage MainWindow::equalizeImage(const QImage &image)
{
    QVector<int> histogramR(256, 0);
    QVector<int> histogramG(256, 0);
    QVector<int> histogramB(256, 0);

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            QRgb pixel = image.pixel(x, y);
            int red = qRed(pixel);
            int green = qGreen(pixel);
            int blue = qBlue(pixel);

            ++histogramR[red];
            ++histogramG[green];
            ++histogramB[blue];
        }
    }

    QVector<int> cumulativeDistributionR(256, 0);
    QVector<int> cumulativeDistributionG(256, 0);
    QVector<int> cumulativeDistributionB(256, 0);

    cumulativeDistributionR[0] = histogramR[0];
    cumulativeDistributionG[0] = histogramG[0];
    cumulativeDistributionB[0] = histogramB[0];

    for (int i = 1; i < 256; ++i)
    {
        cumulativeDistributionR[i] = cumulativeDistributionR[i - 1] + histogramR[i];
        cumulativeDistributionG[i] = cumulativeDistributionG[i - 1] + histogramG[i];
        cumulativeDistributionB[i] = cumulativeDistributionB[i - 1] + histogramB[i];
    }

    QVector<int> normalizedHistogramR(256, 0);
    QVector<int> normalizedHistogramG(256, 0);
    QVector<int> normalizedHistogramB(256, 0);

    for (int i = 0; i < 256; ++i)
    {
        normalizedHistogramR[i] = (cumulativeDistributionR[i] * 255) / (image.width() * image.height());
        normalizedHistogramG[i] = (cumulativeDistributionG[i] * 255) / (image.width() * image.height());
        normalizedHistogramB[i] = (cumulativeDistributionB[i] * 255) / (image.width() * image.height());
    }

    QImage equalizedImage(image.size(), image.format());

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            QRgb pixel = image.pixel(x, y);
            int red = qRed(pixel);
            int green = qGreen(pixel);
            int blue = qBlue(pixel);

            int equalizedRed = normalizedHistogramR[red];
            int equalizedGreen = normalizedHistogramG[green];
            int equalizedBlue = normalizedHistogramB[blue];

            equalizedImage.setPixel(x, y, qRgb(equalizedRed, equalizedGreen, equalizedBlue));
        }
    }

    return equalizedImage;
}
