#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <list>
#include <functional>

class usbDevice
{
public:
    usbDevice(){}
    usbDevice(const std::string& devName, const std::string& linkName, const std::string& storageName,const std::string type):
    deviceName(devName), 
    deviceLink(linkName),
    mountPath("/media/"),
    storageName(storageName),
    fileSysType(type)
    {
        mountPath.append(storageName);
    }
    usbDevice(const char* devName, const char* linkName, const char* storageName, const char* type):
    deviceName(devName), 
    deviceLink(linkName),
    mountPath("/media/"),
    storageName(storageName),
    fileSysType(type)
    {
        mountPath.append(storageName);
    }
    usbDevice(const usbDevice& dev)
    {
        deviceName = dev.deviceName;
        deviceLink = dev.deviceLink;
        mountPath = dev.mountPath;
        storageName = dev.storageName;
        fileSysType = dev.fileSysType;
    }
    usbDevice(usbDevice&& dev):
    deviceName(std::move(dev.deviceName)),
    deviceLink(std::move(dev.deviceLink)),
    mountPath(std::move(dev.mountPath)),
    storageName(std::move(dev.storageName)),
    fileSysType(std::move(dev.fileSysType))
    {
    }
    usbDevice& operator=(const usbDevice& dev)
    {
        deviceName = dev.deviceName;
        deviceLink = dev.deviceLink;
        mountPath = dev.mountPath;
        storageName = dev.storageName;
        fileSysType = dev.fileSysType;
        return *this;
    }
    usbDevice& operator=(usbDevice&& dev)
    {
        deviceName = std::move(dev.deviceName);
        deviceLink = std::move(dev.deviceLink);
        mountPath = std::move(dev.mountPath);
        storageName = std::move(dev.storageName);
        fileSysType = std::move(dev.fileSysType);
        return *this;
    }
    bool operator==(const usbDevice& dev)
    {
        if(deviceLink.compare(dev.deviceLink) == 0 && storageName.compare(dev.storageName) == 0)
        {
            return true;
        }
        return false;
    }
    ~usbDevice()
    {
    }
    std::string getDeviceName() const
    {
        return deviceName;
    }
    std::string getDeviceLink() const
    {
        return deviceLink;
    }
    std::string getMountPath() const
    {
        return mountPath;
    }
    std::string getStorageName() const
    {
        return storageName;
    }
    std::string getFileSysType() const
    {
        return fileSysType;
    }
    void setDeviceName(const std::string& devName)
    {
        deviceName = devName;
    }
    void setDeviceLink(const std::string& devLink)
    {
        deviceLink = devLink;
    }
    void setMountPath(const std::string& mountPh)
    {
        mountPath = mountPh;
    }
    void setStorageName(const std::string& usbName)
    {
        storageName = usbName;
    }
    void setFileSysType(const std::string& fType)
    {
        fileSysType = fType;
    }
private:
    std::string deviceName;
    std::string deviceLink;
    std::string mountPath;
    std::string storageName;
    std::string fileSysType;
};

class MountTool
{
    using MountResultCb = std::function<void(const usbDevice&, const int32_t)> ;
public:
    MountTool();
    ~MountTool();
    MountTool(const MountTool&) = delete;
    MountTool(MountTool&&) = delete;
    MountTool& operator =(const MountTool&) = delete;
    MountTool& operator =(MountTool&&) = delete;

    bool doMountOrUnmount(const std::string& deviceFile, int32_t statu);
    uint32_t getMountedUsbCount() const {return deviceList.size();}
    void getMountedUsbInfo() const;
    bool checkAndMountInsertUsbDev();
    void setMountResultNtfyCb(MountResultCb&& cb)
    {
        resultCb = std::move(cb);
    }
private:
    bool getDeviceInfo(const std::string& deviceFile, int32_t statu, usbDevice& dev);
    bool addDevice(const usbDevice& dev);
    bool removeDevice(const usbDevice& dev);
    bool isUsbDevice(const std::string& deviceFile,  int32_t statu);
    bool isDeviceAlreadyAdded(const usbDevice&  dev);
    bool isMounted(const usbDevice& dev);
    bool isDirectoryExits(const std::string& directory);
    bool createDir(const std::string& directory);
    bool deleteDir(const std::string& directory);
    bool mount(const usbDevice&  dev);
    bool umount(const usbDevice&  dev);
    void doMountEventNotify(const usbDevice& dev, const int32_t event)
    {
        if(resultCb)
        {
            resultCb(dev, event);
        }
    }
private:
    std::list<usbDevice*> deviceList;
    MountResultCb resultCb;
public:
    static const int32_t STATU_INSERT = 0;
    static const int32_t STATU_DELETE = 1;
};