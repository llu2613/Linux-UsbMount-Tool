#include "Command.h"
#include <fstream>
#include <errno.h>
#include <cstring>
#include <unistd.h>
using namespace std;

Command::Command(const std::string& path):fileName(path + RESULT_FILENAME)
{

}
Command::~Command()
{

}
bool Command::runCommand(const std::string& cmd)
{
    char buff[2048] = {0};
    fp = popen(cmd.c_str(), "r");
    if(fp == nullptr)
    {
        return false;
    }
    ssize_t len = fread(buff, 256, 8, fp);
    fclose(fp);
    if(len < 0)
    {
        cout << " Error : read result fail "  << strerror(errno) << endl;
        return false;
    }
    result = std::move(string(buff));
    return true;
}
bool Command::runCommand(const std::vector<std::string>& cmd)
{
    fstream file;
    file.open(fileName, ios::app);
    for (auto & it : cmd)
    {
        if(!runCommand(it))
        {
            cout << " Error : run comand " << it << " fail " << endl;
        }
        else
        {
            if(file.is_open())
            {
                file.write(result.c_str(), result.size());
            }
            else
            {
                cout << result << endl;
            }
        }
    }
    if(file.is_open())
    {
        file.sync();
        ::sync();
        file.close();
    }
    return true;
}