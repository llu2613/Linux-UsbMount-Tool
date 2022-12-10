#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/select.h>
#include <sys/inotify.h>
#include <cstring>
#include "Notify.h"
#include "MountTool.h"

using namespace std;

int main(int argc, char**argv)
{
    fd_set readset;
    FD_ZERO(&readset);

    MountTool mount;
    

    Notify mnotify;
    mnotify.setEventCallBack([&](const inotify_event& e){
        if(e.mask & Notify::NOTIFY_EVENT_IN_CREATE)
        {
            usbDevice test;
            if(mount.doMountOrUnmount(e.name, MountTool::STATU_INSERT))
            {
                mount.getMountedUsbInfo();
            }
        }
        if(e.mask & Notify::NOTIFY_EVENT_IN_DELETE)
        {
            usbDevice test;
            if(mount.doMountOrUnmount(e.name, MountTool::STATU_DELETE))
            {
                mount.getMountedUsbInfo();
            }
        }
    });
    mnotify.addWatchFile("/dev/disk/by-id",Notify::NOTIFY_EVENT_IN_CREATE | Notify::NOTIFY_EVENT_IN_DELETE | Notify::NOTIFY_EVENT_IN_IGNORED | Notify::NOTIFY_EVENT_IN_CLOSE);

    while(true)
    {
        FD_SET(mnotify.getFd(), &readset);
        if(select(mnotify.getFd()+1, &readset, nullptr, nullptr, nullptr) > 0)
        {
            if(FD_ISSET(mnotify.getFd(), &readset))
            {
                mnotify.eventHandle();
            }
        }
    }
    return 0;
}
