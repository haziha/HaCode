# DriveInfo

> 获取存储设备序列号

---

由于使用了COM接口，并且考虑到COM接口初始化限制，导致实际情况中，可能会与其他模块冲突 \
所以使用 QThread 多线程方式，在线程内单独初始化COM接口，并使用信号与槽实现数据传递

### 非阻塞

```c++
connect(this->ui->asyncBtn, &QPushButton::clicked, &this->driveInfo, &::DriveInfo::LoadAllDrive);
connect(&this->driveInfo, &::DriveInfo::finished, [](const QVector<::driveInfo> &drives) {});
```

### 阻塞

```c++
connect(this->ui->syncBtn, &QPushButton::clicked, &this->driveInfo, &::DriveInfo::LoadAllDrive, Qt::BlockingQueuedConnection);
connect(&this->driveInfo, &::DriveInfo::finished, [](const QVector<::driveInfo> &drives) {});
```
