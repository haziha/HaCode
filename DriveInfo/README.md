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

---

## 可能遇到的问题

> ### 显示 `'swprintf_s' was not declared in this scope`
> 将 `iostream` 头文件移到最前面

> ### 显示 `undefined reference to '__imp_Co......'`
> 添加库文件 `-lole32`

> ### 显示 `undefined reference to '__imp_Sys......String'`
> 添加库文件 `-loleaut32`

> ### 显示 `undefined reference to 'CLSID_WbemLocator' 或 undefined reference to 'IID_IWbemLocator'`
> 添加头文件 `initguid` 且该头文件移到最前面  
> 若使用MSVC编译器则要添加 #pragma comment(lib, "wbemuuid.lib")

> ### 显示 `undefined reference to '_com_util::ConvertStringToBSTR(char const*)'`
> 看下面

> ### 最好不要写成 `bstr_t("xxx")` 这种形式
> 应写成 `bstr_t(L"xxx")`  
> 有拼接需求可以写成下面这种  
> ```c++
> std::wstring wstr = L"x";
> wstr += L"xx";
> bstr_t(wstr.c_str());
> ```
> 不推荐用下面这种
> ```c++
> BSTR bstr = SysAllocString(L"xxx");
> SysFreeString(bstr); // 记得执行, 否则内存泄漏
> ```
