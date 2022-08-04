//
// Created by hazih on 2022/7/26.
//

#include <iostream>
#include <initguid.h>
#include <comdef.h>

#include "DriveInfo.h"

#include "Utf8Utf16.h"

#ifdef USE_QDEBUG
#include <QDebug>
#endif

driveInfo::driveInfo(const QString &caption, const QString &deviceID, const QString &serialNumber,
                     const QString &partitionID, const QString &driveLetter,
                     quint32 driveType) {
    this->Caption = caption;
    this->DeviceID = deviceID;
    this->SerialNumber = serialNumber;
    this->PartitionID = partitionID;
    this->DriveLetter = driveLetter;
    this->DriveType = driveType;
    this->empty = false;
}

DriveInfo::DriveInfo(QObject *parent) : QObject(parent) {
    this->thread = new QThread;
    this->moveToThread(this->thread);
    connect(this->thread, &QThread::started, this, &::DriveInfo::InitOnec);
    this->thread->start();

    deferM << [&]() {
        this->thread->quit();
        this->thread->wait();
        this->thread->deleteLater();
    };
}

void DriveInfo::InitOnec() {
    if (this->initSuccess) {
        return;
    }

    HRESULT hres = 0;

    // 初始化COM接口
    hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
#ifdef USE_QDEBUG
        qDebug() << QString("Failed to initialize COM library. Error code = 0x%1").arg(QString::number(hres, 16).toUpper()).toStdString().c_str();
#endif
        return;
    } else {
#ifdef USE_QDEBUG
        qDebug() << "COM library is initialized.";
#endif
    }
    deferMExpr(CoUninitialize(););

    // 设置安全等级
    hres = CoInitializeSecurity(
            nullptr,
            -1,
            nullptr,
            nullptr,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE,
            nullptr);
    if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
#ifdef USE_QDEBUG
        if(RPC_E_NO_GOOD_SECURITY_PACKAGES == hres)
        {
            qDebug() << "RPC_E_NO_GOOD_SECURITY_PACKAGES";
        }
        qDebug() << QString("Failed to initialize security. Error code = 0x%1").arg(QString::number(hres, 16).toUpper()).toStdString().c_str();
#endif
        return;
    }
#ifdef USE_QDEBUG
    qDebug() << "security initialized.";
#endif

    hres = CoCreateInstance(CLSID_WbemLocator,
                            nullptr,
                            CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator,
                            reinterpret_cast<LPVOID *>(&this->pLoc));
    if (FAILED(hres)) {
#ifdef USE_QDEBUG
        qDebug() << QString("Failed to create IWbemLocator object. Error code = 0x%1").arg(QString::number(hres, 16).toUpper()).toStdString().c_str();
#endif
        return;
    } else {
#ifdef USE_QDEBUG
        qDebug() << "The IWbemLocator object is created.";
#endif
    }
    deferM << [&]() {
        if (this->pLoc != nullptr) {
            this->pLoc->Release();
        }
    };

    hres = this->pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            nullptr,
            nullptr,
            nullptr,
            0,
            nullptr,
            nullptr,
            &this->pSvc);
    if (FAILED(hres)) {
#ifdef USE_QDEBUG
        qDebug() << QString("Could not connect. Error code = 0x%1").arg(QString::number(hres, 16).toUpper()).toStdString().c_str();
#endif
        return;
    } else {
#ifdef USE_QDEBUG
        qDebug() << "Connected Server.";
#endif
    }
    deferM << [&]() {
        if (this->pSvc != nullptr) {
            this->pSvc->Release();
        }
    };

    hres = CoSetProxyBlanket(
            this->pSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            nullptr,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            nullptr,
            EOAC_NONE
    );
    if (FAILED(hres)) {
#ifdef USE_QDEBUG
        qDebug() << QString("Could not set proxy blanket. Error code = 0x%1").arg(QString::number(hres, 16).toUpper()).toStdString().c_str();
#endif
        return;
    } else {
#ifdef USE_QDEBUG
        qDebug() << "proxy blanket has been set.";
#endif
    }

#ifdef USE_QDEBUG
    qDebug() << "Congratulations, all initializations are complete.";
#endif

    this->initSuccess = true;
}

void DriveInfo::LoadAllDrive() {
    ULONG uReturn = 0;
    HRESULT hres;
    QVector<driveInfo> localDrives;

    defer << [&]() {
        this->drives = localDrives;
        emit this->finished(localDrives);
    };

    // 使用 Win32_DiskDrive 获取所有存储设备及设备信息
    IEnumWbemClassObject *pEnumDiskDrive = nullptr;
    hres = this->pSvc->ExecQuery(_bstr_t(L"WQL"),
                                 _bstr_t(L"SELECT Caption, DeviceID, SerialNumber FROM Win32_DiskDrive"),
                                 WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                 nullptr,
                                 &pEnumDiskDrive);
    if (FAILED(hres)) {
        return;
    }
    defer << [&]() {
        if (pEnumDiskDrive != nullptr) {
            pEnumDiskDrive->Release();
        }
    };

    // 遍历所有存储设备
    while (pEnumDiskDrive) {
        IWbemClassObject *pDiskDriveObj = nullptr;

        pEnumDiskDrive->Next(static_cast<LONG>(WBEM_INFINITE), 1, &pDiskDriveObj, &uReturn);
        if (uReturn == 0) {
            break;
        }
        defer << [&]() {
            if (pDiskDriveObj != nullptr) {
                pDiskDriveObj->Release();
            }
        };

        VARIANT vtCaption, vtDriveDeviceID, vtSerialNumber;
        VariantInit(&vtCaption);
        VariantInit(&vtDriveDeviceID);
        VariantInit(&vtSerialNumber);
        defer << [&]() {
            VariantClear(&vtCaption);
            VariantClear(&vtDriveDeviceID);
            VariantClear(&vtSerialNumber);
        };

        pDiskDriveObj->Get(L"Caption", 0, &vtCaption, nullptr, nullptr);
        pDiskDriveObj->Get(L"DeviceID", 0, &vtDriveDeviceID, nullptr, nullptr);
        pDiskDriveObj->Get(L"SerialNumber", 0, &vtSerialNumber, nullptr, nullptr);

        // 根据 DiskDrive 的 DeviceID成员 关联 DiskPartition
        // DiskPartiton 的 DeviceID成员 可用于关联 LocalDisk, 从而获取盘符
        std::wstring diskPartitionQuery = L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='";
        diskPartitionQuery += vtDriveDeviceID.bstrVal;
        diskPartitionQuery += L"'} WHERE AssocClass = Win32_DiskDriveToDiskPartition";
        IEnumWbemClassObject *pEnumDiskPartition = nullptr;

        hres = this->pSvc->ExecQuery(_bstr_t(L"WQL"),
                                     _bstr_t(diskPartitionQuery.c_str()),
                                     WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                     nullptr,
                                     &pEnumDiskPartition);
        if (FAILED(hres)) {
            continue;
        }
        defer << [&]() {
            if (pEnumDiskPartition != nullptr) {
                pEnumDiskPartition->Release();
            }
        };

        // 遍历当前设备的所有分区ID
        while (pEnumDiskPartition) {
            IWbemClassObject *pDiskPartitionObj = nullptr;
            pEnumDiskPartition->Next(static_cast<LONG>(WBEM_INFINITE),
                                     1,
                                     &pDiskPartitionObj,
                                     &uReturn);
            if (uReturn == 0) {
                break;
            }
            defer << [&]() {
                if (pDiskPartitionObj != nullptr) {
                    pDiskPartitionObj->Release();
                }
            };

            VARIANT vtPartitionDeviceID;
            VariantInit(&vtPartitionDeviceID);
            deferExpr(VariantClear(&vtPartitionDeviceID););

            pDiskPartitionObj->Get(L"DeviceID",
                                   0,
                                   &vtPartitionDeviceID,
                                   nullptr,
                                   nullptr);

            // 根据 DiskPartition 的 DeviceID成员 关联 LogicalDisk
            // LogicalDisk 有当前分区的盘符
            std::wstring logicalQuery = L"ASSOCIATORS OF {Win32_DiskPartition.DeviceID='";
            logicalQuery += vtPartitionDeviceID.bstrVal;
            logicalQuery += L"'} WHERE AssocClass = Win32_LogicalDiskToPartition";
            IEnumWbemClassObject *pEnumLogicalDisk = nullptr;

            hres = this->pSvc->ExecQuery(_bstr_t(L"WQL"),
                                         _bstr_t(logicalQuery.c_str()),
                                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                         nullptr,
                                         &pEnumLogicalDisk);
            if (FAILED(hres)) {
                continue;
            }
            defer << [&]() {
                if (pEnumLogicalDisk != nullptr) {
                    pEnumLogicalDisk->Release();
                }
            };

            // 遍历 LogicalDisk
            while (pEnumLogicalDisk) {
                IWbemClassObject *pLogicalDiskObj = nullptr;
                pEnumLogicalDisk->Next(static_cast<LONG>(WBEM_INFINITE), 1, &pLogicalDiskObj, &uReturn);
                if (uReturn == 0) {
                    break;
                }
                deferExpr(pLogicalDiskObj->Release(););

                VARIANT vtLogicalDeviceID, vtDriveType;
                VariantInit(&vtLogicalDeviceID);
                VariantInit(&vtDriveType);
                defer << [&]() {
                    VariantClear(&vtLogicalDeviceID);
                    VariantClear(&vtDriveType);
                };

                pLogicalDiskObj->Get(L"DeviceID",
                                     0,
                                     &vtLogicalDeviceID,
                                     nullptr,
                                     nullptr);
                pLogicalDiskObj->Get(L"DriveType",
                                     0,
                                     &vtDriveType,
                                     nullptr,
                                     nullptr);

#ifdef USE_QDEBUG
                qDebug() << ("Caption: " + QString::fromStdString(CodeConversion::Utf16ToUtf8(vtCaption.bstrVal)) +
                             " DeviceID: " + QString::fromStdString(CodeConversion::Utf16ToUtf8(vtDriveDeviceID.bstrVal)) +
                             " SerialNumber: " + QString::fromStdString(CodeConversion::Utf16ToUtf8(vtSerialNumber.bstrVal))).toStdString().c_str();
                qDebug() << ("  PartitionID: " + QString::fromStdString(CodeConversion::Utf16ToUtf8(vtPartitionDeviceID.bstrVal)) +
                             "  DriveLetter: " + QString::fromStdString(CodeConversion::Utf16ToUtf8(vtLogicalDeviceID.bstrVal)) +
                             "  DriveType: " + QString::number(vtDriveType.ulVal)).toStdString().c_str();
#endif
                localDrives.append(driveInfo(QString::fromStdString(Utf8Utf16::Utf16ToUtf8(vtCaption.bstrVal)),
                                             QString::fromStdString(Utf8Utf16::Utf16ToUtf8(vtDriveDeviceID.bstrVal)),
                                             QString::fromStdString(Utf8Utf16::Utf16ToUtf8(vtSerialNumber.bstrVal)),
                                             QString::fromStdString(
                                                     Utf8Utf16::Utf16ToUtf8(vtPartitionDeviceID.bstrVal)),
                                             QString::fromStdString(Utf8Utf16::Utf16ToUtf8(vtLogicalDeviceID.bstrVal)),
                                             vtDriveType.ulVal));
            }
        }
    }

#ifdef USE_QDEBUG
    qDebug() << "load all drive finished";
#endif
}

bool DriveInfo::InitSuccess() const {
    return this->initSuccess;
}
