#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void openImage();
    void saveImage();

private:
    QLabel *originalImageLabel;
    QLabel *equalizedImageLabel;
    QImage equalizedImage;

    QImage equalizeImage(const QImage &image);
};

#endif // MAINWINDOW_H
