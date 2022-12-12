#include "MountTool.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <dirent.h>

using namespace std;

MountTool::MountTool()
{

}
MountTool::~MountTool()
{
    usbDevice* temp = nullptr;
    while(!deviceList.empty())
    {
        temp = deviceList.front();
        if(temp)
        {
            delete temp;
            deviceList.pop_front();
        }
    }
}
bool MountTool::isUsbDevice(const std::string& deviceFile, int32_t statu)
{
    if(deviceFile.find("part") == deviceFile.npos)
    {
        return false;
    }
    if(statu == STATU_INSERT)
    {
        string devpath(32, '\0');
        string devlink("/dev/disk/by-id/");
        devlink.append(deviceFile);
        int ret = readlink(devlink.c_str(), &*devpath.begin(), devpath.size());
        if(ret < 0)
        {
            cout << "Error: get device faile 222" << strerror(errno) << endl;
            return false;
        }
        if(devpath.find("sd") == string::npos)
        {
            return false;
        }
    }
    return true;
}
bool MountTool::getDeviceInfo(const std::string& deviceFile, int32_t statu, usbDevice& dev)
{
    uint32_t pos1 = 0;
    uint32_t pos2 = 0;
    char buff[64] = {0};
    string devlink("/dev/disk/by-id/"); 
    string usbName;
    string devName;
    string fileType = "vfat";

    pos1 = deviceFile.find_first_of('-');
    if(pos1 == string::npos)
    {
        cout << "Error: get pos1 fail " << endl;
        return false;
    }
    pos2 = deviceFile.find_first_of('-',pos1+1);
    if(pos2 == string::npos)
    {
        cout << "Error: get pos2 fail " << endl;
        return false;
    }
    usbName = deviceFile.substr(pos1 + 1, pos2 - pos1 - 1);

    //get dev name
    devlink.append(deviceFile);
    if(statu == MountTool::STATU_INSERT)
    {
        memset(buff, 0, sizeof(buff));
        int ret = readlink(devlink.c_str(), buff, sizeof(buff));
        if(ret < 0)
        {
            cout << "Error: get device faile 111 " << strerror(errno) << endl;
            return false;
        }
        pos1 = string(buff).find_last_of('/');
        if(pos1 == string::npos)
        {
            return false;
        }
        devName.append("/dev/").append(&buff[pos1 + 1]);
    }
    dev.setFileSysType(fileType);
    dev.setDeviceName(devName);
    dev.setDeviceLink(devlink);
    dev.setMountPath(string("/media/").append(usbName));
    dev.setStorageName(usbName);

    return true;
}
bool MountTool::doMountOrUnmount(const std::string& deviceFile, int32_t statu)
{
    usbDevice dev;
    if(!isUsbDevice(deviceFile, statu))
    {
        return false;
    }

    if(!getDeviceInfo(deviceFile, statu, dev))
    {
        return false;
    }

    if(statu == MountTool::STATU_INSERT)
    {        
        if(!isMounted(dev))
        {
            if(mount(dev))
            {
                addDevice(dev);
            }
        }
    }
    else
    {
        if(isMounted(dev))
        {
            if(umount(dev))
            {
                removeDevice(dev);
            }
        }
    }
    return true;
}
bool MountTool::addDevice(const usbDevice& dev)
{
    if(isDeviceAlreadyAdded(dev))
    {
        return false;
    }
    deviceList.push_back(new usbDevice(dev));
    return true;
}
bool MountTool::removeDevice(const usbDevice& dev)
{
    for(auto& dv : deviceList)
    {
        if(*dv == dev)
        {
            deviceList.remove(dv);
            delete dv;
            return true;
        }
    }
    return false;
}
bool MountTool::isDeviceAlreadyAdded(const usbDevice&  dev)
{
    for(auto& tmp : deviceList)
    {
        if(*tmp == dev)
        {
            return true;
        }
    }
    return false;
}
bool MountTool::isMounted(const usbDevice&  dev)
{
    string  readBuff(1024, '\0');
    int32_t fd = -1;
    int32_t len = 0;
    bool mounted = false;
    fd = open("/proc/mounts", O_RDONLY);
    if(fd < 0)
    {
        cout << "Error: open " << "/proc/mounts" << " Fail :" << strerror(errno) << endl;
        return false;
    }
    while((len = read(fd, &*readBuff.begin(), readBuff.size())) > 0)
    {
        if(readBuff.find(dev.getDeviceName()) != string::npos)
        {
            mounted = true;
            break;
        }
    }
    close(fd);
    return mounted;
}
bool MountTool::isDirectoryExits(const std::string& directory)
{
    return (access(directory.c_str(), F_OK) == 0);
}
bool MountTool::createDir(const std::string& directory)
{
    if(mkdir(directory.c_str(), 0777) != 0)
    {
        cout<<"Error mkdir " << directory << " fail : " << strerror(errno) << endl;;
        return false;
    }
    return true;
}
bool MountTool::deleteDir(const std::string& directory)
{
    if(rmdir(directory.c_str()) != 0)
    {
        cout<<"Error rmdir " << directory << " fail : " << strerror(errno) << endl;
        return false;
    }
    return true;
}
bool MountTool::mount(const usbDevice&  dev)
{
    if(dev.getMountPath().size() == 0 || dev.getDeviceName().size() == 0)
    {
        return false;
    }
    if(!isDirectoryExits(dev.getMountPath()))
    {
        if(!createDir(dev.getMountPath()))
        {
            return false;
        }
    }
    if (::mount(dev.getDeviceName().c_str(), dev.getMountPath().c_str(), dev.getFileSysType().c_str() ,0xc0ed0000, "codepage=936,iocharset=gb2312") != 0)
    {
        cout<<"Error mount " << dev.getDeviceName() << " to " << dev.getMountPath() << " fail : " << strerror(errno) << endl;
        deleteDir(dev.getMountPath().c_str());
        return false;
    }
    return true;
}
bool MountTool::umount(const usbDevice&  dev)
{
    if(dev.getMountPath().size() == 0)
    {
        return false;
    }
    if(!isDirectoryExits(dev.getMountPath()))
    {
        return false;
    }
    if(::umount2(dev.getMountPath().c_str(),MNT_DETACH) != 0)
    {
        cout<<"Error Umount " << dev.getMountPath() << " fail : " << strerror(errno) << endl;
        return false;
    }
    if(!deleteDir(dev.getMountPath().c_str()))
    {
        return false;
    }
    return true;
}
bool MountTool::checkAndMountInsertUsbDev()
{
    DIR *dir = nullptr;
    struct dirent *file = nullptr;
    dir = opendir("/dev/disk/by-id");
    if(dir == nullptr)
    {
        cout<<"Error opendir " << "/dev/disk/by-id" << " fail : " << strerror(errno) << endl;
        return false;
    }
    while((file = readdir(dir)) != nullptr)
    {
        if(isUsbDevice(file->d_name,STATU_INSERT))
        {
            doMountOrUnmount(file->d_name, STATU_INSERT);
        }
    }
    closedir(dir);
    dir = nullptr;
    return true;
}
void MountTool::getMountedUsbInfo() const
{
    cout << "Mounted Usb INFO: " << endl;
    cout << "Mounted Usb num: " << getMountedUsbCount() << "\r\n" << endl;
    for(auto& it : deviceList)
    {
        if(it)
        {
            cout << "USB Name: " << it->getStorageName() << endl;
            cout << "USB DevPath: " << it->getDeviceName() << endl;
            cout << "USB MountPath: " << it->getMountPath() << endl;
            cout << "USB FileSystemType: " << it->getFileSysType() << endl;
        }
        cout << " " << endl;
    }
}