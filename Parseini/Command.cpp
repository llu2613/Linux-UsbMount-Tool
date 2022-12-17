#include "Command.h"
#include <fstream>
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
    ssize_t len = fread(buff, 2048, 1, fp);
    fclose(fp);
    if(len <= 0)
    {
        return false;
    }
    result = std::move(string(buff));
    return true;
}
bool Command::runCommand(const std::vector<std::string>& cmd)
{
    for (auto & it : cmd)
    {
        if(!runCommand(it))
        {
            cout << " Error : run comand " << it << " fail " << result << endl;
        }
        else
        {
            cout << result << endl;
        }
    }
    return true;
}