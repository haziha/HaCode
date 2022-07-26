#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(this->ui->asyncBtn, &QPushButton::clicked, &this->driveInfo, &::DriveInfo::LoadAllDrive);
    connect(this->ui->syncBtn, &QPushButton::clicked, &this->driveInfo, &::DriveInfo::LoadAllDrive,
            Qt::BlockingQueuedConnection);
    connect(&this->driveInfo, &::DriveInfo::finished, [](const QVector<::driveInfo> &drives) {
                foreach(auto drive, drives) {
                qDebug() << ("Caption: " + drive.Caption +
                             " DeviceID: " + drive.DeviceID +
                             " SerialNumber: " + drive.SerialNumber).toStdString().c_str();
                qDebug() << ("  PartitionID: " + drive.PartitionID +
                             "  DriveLetter: " + drive.DriveLetter +
                             "  DriveType: " + QString::number(drive.DriveType)).toStdString().c_str();
            }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}
