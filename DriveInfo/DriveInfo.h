//
// Created by haziha on 2022/7/26.
//

#ifndef DRIVEINFO_DRIVEINFO_H
#define DRIVEINFO_DRIVEINFO_H

#include <wbemidl.h>

#include <QObject>
#include <QThread>
#include <QVector>

#include "DeferClass.h"

class driveInfo {
public:
    driveInfo() = default;

    explicit driveInfo(const QString &caption, const QString &deviceID, const QString &serialNumber,
                       const QString &partitionID,
                       const QString &driveLetter, quint32 driveType);

    QString Caption;
    QString DeviceID;
    QString SerialNumber;
    QString PartitionID;
    QString DriveLetter;
    quint32 DriveType{};

private:
    bool empty = true;
};

class DriveInfo : public QObject {
Q_OBJECT
    USE_DEFER_MEMBER

public:
    explicit DriveInfo(QObject *parent = nullptr);

signals:

    void finished(QVector<driveInfo> drives);

public slots:

    void InitOnec(); // 初始化 COM 接口, 只能调用一次

    bool InitSuccess() const; // 是否初始化成功

    void LoadAllDrive();

private:
    QThread *thread;

    bool initSuccess = false;

    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    QVector<driveInfo> drives;
};


#endif //DRIVEINFO_DRIVEINFO_H
