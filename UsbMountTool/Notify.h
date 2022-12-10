#pragma once

#include <iostream>
#include <string>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <error.h>
#include <sys/inotify.h>
#include <functional>

struct NotifyObj
{
    NotifyObj(int32_t fd, const std::string& str, const int32_t ev):mfd(fd),filePath(str), event(ev)
    {

    }
    NotifyObj(const std::string& str):mfd(-1),filePath(str), event(0)
    {

    }
    NotifyObj(const int32_t fd):mfd(fd),filePath(""), event(0)
    {

    }
    NotifyObj(const NotifyObj&) = default;
    NotifyObj(NotifyObj&& obj)
    {
        mfd = obj.mfd;
        filePath = std::move(obj.filePath);
        event = obj.event;
    }
    int32_t getFd() const
    {
        return mfd;
    }
    std::string getFilePath() const
    {
        return filePath;
    }
    int32_t getEvent()
    {
        return event;
    }
    bool operator==(const NotifyObj& obj)
    {
        if(obj.filePath == filePath || obj.mfd == mfd)
        {
            return true;
        }
        return false;
    }
private:
    int32_t mfd;
    std::string filePath;
    int32_t event;
};

class Notify
{
    using NotifyCallBack = std::function<void(const inotify_event& e)>;
public:
    Notify(int32_t mode = IN_NONBLOCK | IN_CLOEXEC);
    ~Notify();
    Notify(const Notify&) = delete;
    Notify(Notify&&) = delete;
    Notify& operator=(const Notify&) = delete;
    Notify& operator=(Notify&&) = delete;
    int32_t addWatchFile(const std::string& str, int32_t event);
    bool rmWatchFile(const std::string& str);
    bool rmWatchFile(const int32_t fd);
    int32_t getFd() {return fd;}
    void setEventCallBack(const NotifyCallBack& cb){eventCb = cb;}
    void eventHandle();
private:
    int32_t fd;
    NotifyCallBack eventCb;
    unsigned char buff[4096];
    std::list<NotifyObj> objlist;
public:
    //文件被访问(执行了 read 操作)
    static const int32_t NOTIFY_EVENT_IN_ACCESS = IN_ACCESS;
    //文件元数据发生变更
    static const int32_t NOTIFY_EVENT_IN_ATTRIB = IN_ATTRIB;
    //关闭为了写入而打开的文件
    static const int32_t NOTIFY_EVENT_IN_CLOSE_WRITE = IN_CLOSE_WRITE;
    //关闭以只读方式打开的文件
    static const int32_t NOTIFY_EVENT_IN_CLOSE_NOWRITE = IN_CLOSE_NOWRITE;
    //在受控目录内创建了文件或者目录
    static const int32_t NOTIFY_EVENT_IN_CREATE = IN_CREATE;
    //在受控目录内删除了文件或者目录
    static const int32_t NOTIFY_EVENT_IN_DELETE = IN_DELETE;
    //删除受控文件或者目录本身
    static const int32_t NOTIFY_EVENT_IN_DELETE_SELF = IN_DELETE_SELF;
    //文件移出受控目录之外
    static const int32_t NOTIFY_EVENT_IN_MOVED_FROM = IN_MOVED_FROM;
    //文件移入受控目录
    static const int32_t NOTIFY_EVENT_IN_MOVED_TO = IN_MOVED_TO;
    //文件被打开
    static const int32_t NOTIFY_EVENT_IN_OPEN = IN_OPEN;
    //IN_MOVED_FROM|IN_MOVED_TO 事件的统称
    static const int32_t NOTIFY_EVENT_IN_MOVE = IN_MOVE;
    //IN_CLOSE_WRITE|IN_CLOSE_NOWRITE 统称
    static const int32_t NOTIFY_EVENT_IN_CLOSE = IN_CLOSE;

    //控制监听过程的MASK:
    //不对符号链接引用
    static const int32_t NOTIFY_EVENT_IN_DONT_FOLLOW = IN_DONT_FOLLOW;
    //将事件追加到 pathname 的当前监控掩码
    static const int32_t NOTIFY_EVENT_IN_MASK_ADD = IN_MASK_ADD;
    //只监控 pathname 的一个事件
    static const int32_t NOTIFY_EVENT_IN_ONESHOT = IN_ONESHOT;
    //pathname 不为目录时会失败
    static const int32_t NOTIFY_EVENT_IN_ONLYDIR = IN_ONLYDIR;

    //系统异常时会发出的通知，不需要用户手动添加：
    //监控项为内核或者应用程序移除
    static const int32_t NOTIFY_EVENT_IN_IGNORED = IN_IGNORED;
    //被监听的是一个目录的路径
    static const int32_t NOTIFY_EVENT_IN_ISDIR = IN_ISDIR;
    //事件队列溢出
    static const int32_t NOTIFY_EVENT_IN_Q_OVERFLOW = IN_Q_OVERFLOW;
    //包含对象的文件系统遭到卸载
    static const int32_t NOTIFY_EVENT_IN_UNMOUNT = IN_UNMOUNT;
};