#include "Notify.h"
#include <unistd.h>
#include <algorithm>

using namespace std;
    // int32_t fd;
    // NotifyCallBack eventCb;
    // unsigned char buff[4096];
Notify::Notify(int32_t mode):fd(inotify_init1(mode))
{
    memset(buff, 0, sizeof(buff));
}
Notify::~Notify()
{
    for(auto& l : objlist)
    {
        rmWatchFile(l.getFd());
    }
    if(fd > 0)
    {
        close(fd);
        fd = -1;
    }
}
int32_t Notify::addWatchFile(const std::string& str, int32_t event)
{   
    int ifd = -1;
    list<NotifyObj>::iterator it = find(objlist.begin(), objlist.end(), NotifyObj(str));
    if(it != objlist.end())
    {
        if(it->getEvent() != event)
        {
            rmWatchFile(str);
        }
        else
        {
            cout << "this fiel["<< str <<"]" << "already added" << endl;
            return -1;
        }
    }
    ifd = inotify_add_watch(fd, str.c_str(), event);
    if(ifd < 0)
    {
        return ifd;
    }
    objlist.push_back(NotifyObj(ifd, str, event));
    return ifd;
}
bool Notify::rmWatchFile(const std::string& str)
{
    list<NotifyObj>::iterator it = find(objlist.begin(), objlist.end(), NotifyObj(str));
    if(it != objlist.end())
    {
        if(inotify_rm_watch (fd, it->getFd()) < 0)
        {
            return false;
        }
        objlist.erase(it);
    }
    return true;
}
bool Notify::rmWatchFile(const int32_t fd)
{
    list<NotifyObj>::iterator it = find(objlist.begin(), objlist.end(), NotifyObj(fd));
    if(it != objlist.end())
    {
        if(inotify_rm_watch (fd, it->getFd()) < 0)
        {
            return false;
        }
        objlist.erase(it);
    }
    return true;
}
void Notify::eventHandle()
{
    ssize_t len = 0;
    unsigned char* ptr = nullptr;
    const inotify_event* e = (const inotify_event*)buff;
    while((len = read(fd, buff, sizeof(buff))) > 0)
    {
        for(ptr = buff; ptr < buff + len; ptr += sizeof(inotify_event) + e->len)
        {
            e = (const inotify_event*)ptr;
            if(eventCb)
            {
                eventCb(*e);
            }
        }
    }
}